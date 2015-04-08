//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU Lesser General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
// 
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU Lesser General Public License for more details.
// 
// You should have received a copy of the GNU Lesser General Public License
// along with this program.  If not, see http://www.gnu.org/licenses/.
// 

#include "SimHelper.h"
#include <iostream>
#include <fstream>
#include <float.h>
#include <FindModule.h>
//#include <BaseMobility.h>
#include "StaticMobility.h"
#include "StaticNetwLayer.h"
#include "WiseRouteEx.h"
#include "IDSSimpleLayer.h"

Define_Module(SimHelper);

using namespace std;

const simsignalwrap_t SimHelper::mobilityStateChangedSignal =
		simsignalwrap_t(MIXIM_SIGNAL_MOBILITY_CHANGE_NAME);

void SimHelper::initialize(int stage){
    debugEV << "SimHelper is being initialized....." << endl;
	BaseModule::initialize(stage);
	if (stage==0) {

		debug = par("debug").boolValue();
		network = FindModule<>::findNetwork(this);
		debugEV << "Simhelper debugging is on." << endl;

	} else if (stage==1) {
		// read mobility information from file
		initMobility();
		// read routing information from file
		initRouting();

		// register mobility changes on all nodes
		int nodeID = 0;
		cModule* node;

		do {
			if ((node = network->getSubmodule("node", nodeID)) != NULL){
				//FindModule<>::findHost(node)->subscribe(MIXIM_SIGNAL_MOBILITY_CHANGE_NAME, this);
				node->subscribe(mobilityStateChangedSignal, this);
				nodeID++;
			}
		} while (node);

	}
}

void SimHelper::finish(){
    //Martin
    ofstream ofstr ("results/rssi.txt");
    if (ofstr.is_open()){
        ofstr.precision(10);
        std::pair<double,double> rssiPair;
        while (!rssiList.empty()) {
            rssiPair = rssiList.front();
            ofstr << rssiPair.first << "," << rssiPair.second << endl;
            rssiList.pop_front();
        }

        ofstr.close();
    }
//
//
	if (debug){
		writeTopology();
		writeIDSForwardersStats();
		writeNeighbors();

		// Compute statisticscar
		int nodeID = 0;
		cModule* node;
		IDSSimpleLayer* ids;
		StaticNetwLayer* netw;

		// Stats
		cLongHistogram numNeighbors, destNeighbours, packetsSomebody2Somebody;
		map<unsigned int, bool> isAttacker;
		unsigned int linksWhite2White = 0, linksWhite2Black = 0, linksBlack2Black = 0;

		// Get all attackers
		nodeID = 0;
		do {
			node = network->getSubmodule("node", nodeID);
//			netw = node!=NULL?FindModule<StaticNetwLayer*>::findSubModule(node):NULL;
//			if (netw != NULL)
//				isAttacker[nodeID] = netw->pPacketDropping>0;
			nodeID++;
		} while (node);

		// For each node
		nodeID = 0;
		do {
			node = network->getSubmodule("node", nodeID);
			ids = node!=NULL?FindModule<IDSSimpleLayer*>::findSubModule(node):NULL;

			// Collect data
			if (ids != NULL){
				packetsSomebody2Somebody.collect(ids->numSomebody2Somebody);
				numNeighbors.collect(ids->neighborsSet.size());

				for (set<int>::iterator n=ids->neighborsSet.begin(); n!=ids->neighborsSet.end(); n++){
					int neighAddr = (*n);
					destNeighbours.collect(getNodeDistance(nodeID, neighAddr));

					if (!isAttacker[nodeID] && !isAttacker[neighAddr])
						linksWhite2White++;
					if ((isAttacker[nodeID] && !isAttacker[neighAddr]) || (!isAttacker[nodeID] && isAttacker[neighAddr]))
						linksWhite2Black++;
					if (isAttacker[nodeID] && isAttacker[neighAddr])
						linksBlack2Black++;

				}
			}

			nodeID++;
		} while (node);
//
		// Record data
		numNeighbors.recordAs("numNeighbors");
		recordScalar("neighbors in average", numNeighbors.getMean());
//		destNeighbours.recordAs("destNeighbors");
//		packetsSomebody2Somebody.recordAs("packetsSomebody2Somebody");
//		recordScalar("linksWhite2White", linksWhite2White);
//		recordScalar("linksWhite2Black", linksWhite2Black);
//		recordScalar("linksBlack2Black", linksBlack2Black);
	}

	// Write metrics to a specific file necessary for optimization
	writeMetrics();

}

/**
 * Writes metrics file
 */
void SimHelper::writeMetrics() {
    cModule* node;
    IDSSimpleLayer* ids;
    StaticNetwLayer* netw;
    string idsMetricsFile = par("idsMetricsFile");
    int genNumber = par("genNumber");
    int indiNumber = par("indiNumber");
    ofstream ofstr (par("idsMetricsFile"));
    ofstr.precision(10);

    map<unsigned int, bool> isAttacker;
    map<unsigned int, unsigned int> detectedAsAttacker;
    map<unsigned int, unsigned int> detectedAsNonAttacker;
    map<unsigned int, unsigned int> hasNeighbors;
    unsigned int numAttackers = 0;
    double mem = 0;

    // Get all attackers and stats
    unsigned int nodeID = 0;
    do {
        node = network->getSubmodule("node", nodeID);
        ids = node!=NULL?FindModule<IDSSimpleLayer*>::findSubModule(node):NULL;
        netw = node!=NULL?FindModule<StaticNetwLayer*>::findSubModule(node):NULL;

        if (netw != NULL){
            isAttacker[nodeID] = netw->pPacketDropping>0;
            if (isAttacker[nodeID])
                numAttackers++;
        }

        if (ids != NULL && netw != NULL) {
            for (IDSMap::iterator it=ids->forwardersMap.begin();it!=ids->forwardersMap.end();it++){

                IDSEntry* entry = &(it->second);
                const int nodeAddr = it->first;


                // update detection stats
                if (entry->isDropperGlobal){
                    detectedAsAttacker[nodeAddr]++;
                } else {
                    detectedAsNonAttacker[nodeAddr]++;
                }
            }

            // Add consumed memory for this node
            mem = mem + ( (8 * (int)(ids->par("maxMonitoredNodes"))) + (16 * (int)(ids->par("fwdBufferSize"))) );
            // Add neigh count
            hasNeighbors[nodeID] = ids->neighborsSet.size();
        }

        nodeID++;
    } while (node);

    // For having a correct number of nodes:
    nodeID--;


    if (ofstr.is_open()){

        double fn = 0;
        double fp = 0;

        // TODO: Tady dopocitavat ty statistiky dle paperu a zapsat je do souboru !!!
        for (unsigned int i = 0; i < nodeID; i++) {
//            ofstr << "Node " << i << " is attacker: " << isAttacker[i] << endl;
//            ofstr << "Number of nodes detecting " << i << " as attacker: " << detectedAsAttacker[i] << endl;
//            ofstr << "Number of nodes detecting " << i << " as benign: " << detectedAsNonAttacker[i] << endl;

            // TODO: What should we do with nodes that were not detected at all?
            if ( ( detectedAsAttacker[i] + detectedAsNonAttacker[i] ) > 0 ) {
                if (isAttacker[i]) {
                    fn = fn + ((double)(hasNeighbors[i] - detectedAsAttacker[i]) / hasNeighbors[i]);
                } else {
                    fp = fp + ((double)detectedAsAttacker[i] / hasNeighbors[i]);
                }
            }

        }

        if (numAttackers > 0) {
            fn = (double)fn / numAttackers;
        } else {
            fn = 0;
        }

        if (nodeID - numAttackers > 0)
            fp = (double)fp / (nodeID - numAttackers);
        else
            fp = 0;

        if (nodeID > 0)
            mem = mem / (double)nodeID;

        node = network->getSubmodule("node", 1);
        ids = node!=NULL?FindModule<IDSSimpleLayer*>::findSubModule(node):NULL;

        ofstr << "[SETTINGS]" << endl;
        ofstr << "maxMonitoredNodes = " << (int)ids->par("maxMonitoredNodes") << endl;
        ofstr << "fwdBufferSize = " << (int)ids->par("fwdBufferSize") << endl;
        ofstr << "fwdMinPacketsReceived = " << (int)ids->par("fwdMinPacketsReceived") << endl;
        ofstr << "fwdDetectionThreshold = " << (double)ids->par("fwdDetectionThreshold") << endl;
        ofstr << "windowSize = " << (int)ids->par("windowSize") << endl;
        ofstr << "minVotesReceived = " << (int)ids->par("minVotesReceived") << endl;
        ofstr << "votingThreshold = " << (double)ids->par("votingThreshold") << endl;
        ofstr << "[RESULTS]" << endl;
        ofstr << "GenerationNumber = " << genNumber << endl;
        ofstr << "IndividualNumber = " << indiNumber << endl;
        ofstr << "NumberOfNodes = " << nodeID << endl;
        ofstr << "FN = " << fn << endl;
        ofstr << "FP = " << fp << endl;
        ofstr << "MEM = " << mem << endl;
        for (int i = 0; i <= nodeID; i++) {
            if (isAttacker[i]) {
                ofstr << i << " TrueAttacker = " << detectedAsAttacker[i] << endl;
                ofstr << i << " FalseNonAttacker = " << detectedAsNonAttacker[i] << endl;
            } else {
                ofstr << i << " FalseAttacker = " << detectedAsAttacker[i] << endl;
                ofstr << i << " TrueNonAttacker = " << detectedAsNonAttacker[i] << endl;
            }
        }

        ofstr.close();
    }
}

/**
 * Writes topology stats into topologyStatsFile
 */
void SimHelper::writeTopology(){
	if (par("topologyStatsFile").str().empty()) return;

	ofstream ofstr (par("topologyStatsFile"));
	if (ofstr.is_open()){
		ofstr.precision(3);
		double scale = 7;
//		double scale = 5;

		ofstr << "digraph topology {" << endl;

		ofstr << "\tgraph [fontsize = 12, outputorder=\"edgesfirst\", size=\""
				<< getParentModule()->par("playgroundSizeX").doubleValue()/scale
				<<"," << getParentModule()->par("playgroundSizeY").doubleValue()/scale
				<< "\"];" << endl;
		ofstr << "\tnode [color = \"/pastel16/3\", penwidth = 1];" << endl;
		ofstr << "\tedge [fontsize = 10, arrowsize = 0.5];" << endl;

		int nodeID = 0;
		cModule* node;
		IDSSimpleLayer* ids;
		StaticNetwLayer* netwStatic;
        WiseRouteEx* netwWise;

		do {
			node = network->getSubmodule("node", nodeID);
			if (node != NULL){
				ids = FindModule<IDSSimpleLayer*>::findSubModule(node);
//				netw = FindModule<StaticNetwLayer*>::findSubModule(node);
		        if (par("networkType").str() == "\"StaticNetwLayer\"") {
		            netwStatic = FindModule<StaticNetwLayer*>::findSubModule(node);
		        }
		        if (par("networkType").str() == "\"WiseRouteEx\"") {
		            netwWise = FindModule<WiseRouteEx*>::findSubModule(node);
		        }

				Coord pos1 = getNodePos(nodeID);

				ofstr << "\t" << nodeID << " [";
				ofstr << "label = \"";
				ofstr << nodeID;
				ofstr << "\"";

				stringstream tmp;
				tmp << ", pos=\"" << getNodePos(nodeID).x/scale << "," << getNodePos(nodeID).y/scale << "!\"";
				/*
				if (attackerInfo.type != ATTACKER_NONE)
					tmp<<", color = \"/pastel16/1\"";
				else if (node->par("isBaseStation").boolValue())
					tmp<<", color = \"/pastel16/2\"";
				*/
				ofstr << tmp.str();
				ofstr << "]" << endl;

				if (par("networkType").str() == "\"StaticNetwLayer\"")
				    if (netwStatic != NULL && netwStatic->getNextHopAddr() != -1)
			            ofstr << "\t" << nodeID << " -> " << netwStatic->getNextHopAddr() << " [color=black, ]" << endl;

				if (par("networkType").str() == "\"WiseRouteEx\"")
				    if (netwWise != NULL && netwWise->getNextHopAddr() != -1)
			            ofstr << "\t" << nodeID << " -> " << netwWise->getNextHopAddr() << " [color=black, ]" << endl;

				if (ids != NULL){
					for (std::set<int>::iterator it=ids->neighborsSet.begin(); it!=ids->neighborsSet.end(); it++){
						int nodeAddr = (*it);

						ofstr << "\t" << nodeAddr << " -> " << nodeID << " [color=lightgrey, arrowhead=empty, penwidth=0.5, label=\"" ;
						//ofstr << getNodeDistance(nodeID, nodeAddr) << "m";
						ofstr << "\"";
						ofstr << "]" << endl;
					}
				}
			}
			nodeID++;
		} while (node);

		ofstr << "}" << endl;
		ofstr.close();
	}
}


/**
 * Writes neighbor stats into resultNeighborsFile
 */
void SimHelper::writeNeighbors(){
//	if (par("neighbourStatsFile").str().empty()) return;
//
//	ofstream ofstr (par("neighbourStatsFile"));
//	if (ofstr.is_open()){
//		ofstr.precision(3);
//
//		int nodeID = 0;
//		cModule* node;
//		IDSSimpleLayer* ids;
//		do {
//			node = network->getSubmodule("node", nodeID);
//			ids = node!=NULL?FindModule<IDSSimpleLayer*>::findSubModule(node):NULL;
//
//			if (ids != NULL){
//				for (std::set<int>::iterator it=ids->neighborsSet.begin(); it!=ids->neighborsSet.end(); it++){
//					int nodeAddr = (*it);
//					ofstr
//						<< nodeID << ";"
//						<< nodeAddr << ";"
//						<< getNodeDistance(nodeID, nodeAddr) << ";"
//						<< endl;
//				}
//			}
//			nodeID++;
//		} while (node);
//
//		ofstr.close();
//	}
}

/**
 * Writes IDS stats (selective forwarders) into resultIDSForwardersFile
 */
void SimHelper::writeIDSForwardersStats(){
//	if (par("idsForwardersStatsFile").str().empty()) return;
//
//	ofstream ofstr (par("idsForwardersStatsFile"));
//	if (ofstr.is_open()){
//		ofstr.precision(3);
//
//		int nodeID = 0;
//		cModule* node;
//		IDSSimpleLayer* ids;
//		do {
//			node = network->getSubmodule("node", nodeID);
//			ids = node!=NULL?FindModule<IDSSimpleLayer*>::findSubModule(node):NULL;
//
//			if (ids != NULL){
//				for (IDSMap::iterator it=ids->forwardersMap.begin();it!=ids->forwardersMap.end();it++){
//					IDSEntry* entry = &(it->second);
//					const int nodeAddr = it->first;
//
//					ofstr
//						<< nodeID << ";"
//						<< nodeAddr << ";"
//						<< entry->packetsReceived << ";"
//						<< entry->packetsForwarded << ";"
//						<< getNodeDistance(nodeID, nodeAddr) << ";"
//						<< entry->getPacketsDroppedRatio() << ";"
//						<< endl;
//				}
//			}
//			nodeID++;
//		} while (node);
//
//		ofstr.close();
//	}
}

void SimHelper::initMobility(){
	const char *mobilityFile = par("mobilityFile");
	unsigned int countLoaded = 0;

	cModule* node;
	StaticMobility* mobility;

	ifstream ifstr (mobilityFile);
	if (ifstr.is_open()){
		string line;
		int id;
		double x,y,z;

		while (ifstr.good()){
			ifstr >> line;

			unsigned int tmp = line.find(";", 0);
			if (tmp==string::npos) continue;
			id = atoi(line.substr(0,tmp).c_str());

			line = line.substr(tmp+1,line.length());
			if ((tmp = line.find(";", 0))==string::npos) continue;
			x = atof(line.substr(0,tmp).c_str());

			line = line.substr(tmp+1,line.length());
			if ((tmp = line.find(";", 0))==string::npos) continue;
			y = atof(line.substr(0,tmp).c_str());

			line = line.substr(tmp+1,line.length());
			z = atof(line.c_str());

			node = getParentModule()->getSubmodule("node", id);
			if (!node) continue;

			mobility = FindModule<StaticMobility*>::findSubModule(node);
			if (!mobility) continue;
			debugEV << "Current position of node " << id << " is: " << mobility->getCurrentPosition() << endl;

			// This does work if "initFromDisplayString == false", see: "void MobilityBase::initializePosition()":
			mobility->par("initialX") = x;
			mobility->par("initialY") = y;
			mobility->par("initialZ") = z;

			// This does not work if "initFromDisplayString == false", see: "void MobilityBase::initializePosition()":
			mobility->setPosition(Coord(x, y, z));

			// Note: proper usage requires changes in MobilityBase.cc:
			// else if (hasPar("initialX") && hasPar("initialY") && hasPar("initialZ"))
			// Nahradit za:
			// if (hasPar("initialX") && hasPar("initialY") && hasPar("initialZ"))

			//nodePositions[id] = Coord(x,y,z);	 alternative
			countLoaded++;
		}
		ifstr.close();
	}
	debugEV << "Loaded " << countLoaded << " items from mobilityFile " << mobilityFile << endl;
}


void SimHelper::initRouting(){
	const char *routingFile = par("routingFile");
	unsigned int countLoaded = 0;

	int nextHopAddr, sinkAddr;

	ifstream ifstr (routingFile);
	if (ifstr.is_open()){
		string line;

		while (ifstr.good()){
			ifstr >> line;

			unsigned int tmp = line.find(";", 0);
			if (tmp==string::npos) continue;

			int id = atoi(line.substr(0,tmp).c_str());
			line = line.substr(tmp+1,line.length());

			tmp = line.find(";", 0);
			if (tmp==string::npos) continue;

			nextHopAddr = atoi(line.substr(0,tmp).c_str());
			line = line.substr(tmp+1,line.length());
			sinkAddr = atoi(line.c_str());

			cModule* node = getParentModule()->getSubmodule("node", id);
			if (!node) continue;

			StaticNetwLayer* netw = FindModule<StaticNetwLayer*>::findSubModule(node);
			if (!netw) continue;

			if (netw->par("readFromFile").boolValue())
				netw->setNextHopAddr(nextHopAddr);

			debugEV << "Next hop for node " << id << " is " << nextHopAddr << endl;

			countLoaded++;
		}
		ifstr.close();
	}
	debugEV << "Loaded " << countLoaded << " items from routeFile " << routingFile << endl;
}

//bool SimHelper::hasNodePos(int nodeID){
//	return nodePositions.count(nodeID)>0;
//}

const Coord SimHelper::getNodePos(int nodeID){
//	if (hasNodePos(nodeID))
//		return nodePositions[nodeID];
//	else
//		return NULL;
    StaticMobility* mobility;
    cModule* node;
    node = network->getSubmodule("node", nodeID);
    mobility = FindModule<StaticMobility*>::findSubModule(node);

    return mobility->getCurrentPosition();

}

double SimHelper::getNodeDistance(int nodeA, int nodeB){
//	if (hasNodePos(nodeA) && hasNodePos(nodeB))
		return getNodePos(nodeA).distance(getNodePos(nodeB));
//	else
//		return DBL_MAX;
}

void SimHelper::receiveSignal(cComponent *source, simsignal_t signalID, cObject *obj){
//	BaseModule::receiveSignal(source, signalID, obj);
//
//	if (signalID == mobilityStateChangedSignal){
//		BaseMobility* const mobility = check_and_cast<BaseMobility*>(obj);
//		cModule* node = FindModule<>::findHost(mobility);
//		Coord pos = mobility->getCurrentPosition();
//		nodePositions[node->getIndex()] = pos;
//		debugEV << "Node " << node->getIndex() << " changed position to " << pos << endl;
//	}
}
