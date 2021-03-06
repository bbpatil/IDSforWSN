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

#include <omnetpp.h>
#include <sstream>
#include <string>
#include <FindModule.h>
#include "IDSSimpleLayer.h"
#include "StaticNetwLayer.h"
#include "StaticNetwPkt_m.h"
#include "MacPkt_m.h"
//Martin
#include "PhyToMacControlInfo.h"
#include "DeciderResult802154Narrow.h"
#include "Decider802154Narrow.h"

#include "IDSVotingPkt_m.h"
#include "IDSVotingResponsePkt_m.h"
#include "NetwControlInfo.h"

Define_Module(IDSSimpleLayer);

using namespace std;

/**
 * Initialize module
 */
void IDSSimpleLayer::initialize(int stage){
    debugEV << "IDS is being initialized..." << endl;
	IDSLayer::initialize(stage);
	if (stage==0){
	    idsSenderControlOut = findGate("idsSenderControlOut");

		net = FindModule<StaticNetwLayer*>::findSubModule(getNode());
		helper = FindModule<SimHelper*>::findGlobalModule();
		maxDistance = par("maxDistance").doubleValue();
		maxMonitoredNodes = par("maxMonitoredNodes").longValue();
		fwdBufferSize = par("fwdBufferSize").longValue();
		fwdMinPacketsReceived = par("fwdMinPacketsReceived").longValue();
		fwdPacketTimeout = par("fwdPacketTimeout").doubleValue();
		fwdDetectionThreshold = par("fwdDetectionThreshold").doubleValue();

		fullStats = par("fullStats").boolValue();

		collectingTime = par("collectingTime").longValue();

		usedMemory = maxMonitoredNodes * par("memIDSEntry").doubleValue() + fwdBufferSize * par("memBufferEntry").doubleValue();
		maxNeighbor = -1;
		numInterceptedPackets = 0;
		numSomebody2Me = numSomebody2Somebody = numMe2Somebody = 0;
		evaluated = false;

		// New parameters:
		windowSize = par("windowSize");
		minVotesReceived = par("minVotesReceived");
		votingThreshold = par("votingThreshold").doubleValue();

		// Start the timer for window size:
		idsWindowTimer = new cMessage("ids-window-timer", IDS_WINDOW_TIMER);
		// Once the first window is over, we utilize only partial information from such shorter window
		scheduleAt(simTime() + windowSize + uniform(0,windowSize), idsWindowTimer);
	}
}

/**
 * Handle messages from upper (network) layer
 */
void IDSSimpleLayer::handleUpperMsg(cMessage *msg){
	MacPkt* macPkt = static_cast<MacPkt *> (msg);

	StaticNetwPkt* netwPkt = dynamic_cast<StaticNetwPkt *> (macPkt->getEncapsulatedPacket());
	if (netwPkt)
		analyseNetwPkt(netwPkt);

    debugEV << "IDS handles upper msg!!!" << endl;

	IDSLayer::handleUpperMsg(msg);
}

/**
 * Handle messages from lower (physical) layer
 */
void IDSSimpleLayer::handleLowerMsg(cMessage *msg){

    debugEV << "IDS handles lower msg.." << endl;

	MacPkt* macPkt = static_cast<MacPkt *> (msg);

//	debugEV << "message kind " << msg->getKind() << endl;
//	debugEV << "message kind " << macPkt->getKind() << endl;

    // Martin - this can be used to monitor RSSIs of the packets together with distance -> to find out anomalies observed earlier.
//    PhyToMacControlInfo* cinfo = static_cast<PhyToMacControlInfo*> (macPkt->getControlInfo());
//    const DeciderResult802154Narrow* result = static_cast<const DeciderResult802154Narrow*> (cinfo->getDeciderResult());
//    //double ber = result->getBER();
//    double rssi = result->getRSSI();
//    EV << "RSSI on the IDS layer is " << rssi << " ( " << FWMath::mW2dBm(rssi) << " dBm)." << endl;
//    EV << "Source is " << macPkt->getSrcAddr().getInt() << " and the distance is " << helper->getNodeDistance(macPkt->getSrcAddr().getInt(), int(net->getNetwAddr()) ) << " m." << endl;
//    helper->rssiList.push_back( std::make_pair(helper->getNodeDistance(macPkt->getSrcAddr().getInt(), int(net->getNetwAddr()) ), FWMath::mW2dBm(rssi) ) );
//    EV << "The size of the list is " << helper->rssiList.size() << endl;
    // nitraM

	// Chceck whether the packet is network -> should be analysed
    StaticNetwPkt* netwPkt = dynamic_cast<StaticNetwPkt *> (macPkt->getEncapsulatedPacket());
	if (netwPkt) {
	    debugEV << "Network packet will be analysed by IDS." << endl;
		analyseNetwPkt(netwPkt);

	// Check whether the packet is ids voting -> should be responded
	} else {
	    IDSVotingPkt* idsPkt = dynamic_cast<IDSVotingPkt *> ( (dynamic_cast<NetwPkt *> (macPkt->getEncapsulatedPacket()))->getEncapsulatedPacket() );
	    if (idsPkt) {
	        debugEV << "We decapsulated IDS voting packet." << endl;
	        //debugEV << "Node " << macPkt->getSrcAddr().getInt() << " requires our claim on node " <<  idsPkt->getNodeID() << endl;
	        // Voting response is required
	        votingResponse(idsPkt);

	        delete msg;
	        return;

	    // Check whether the packet is ids response (vote) -> should be recorded
	    } else {
	        IDSVotingResponsePkt* idsResponsePkt = dynamic_cast<IDSVotingResponsePkt *> ( (dynamic_cast<NetwPkt *> (macPkt->getEncapsulatedPacket()))->getEncapsulatedPacket() );
	        if (idsResponsePkt) {
	            debugEV << "We decapsulated IDS voting response packet." << endl;
	            if (idsResponsePkt->getDestAddr() == net->getNetwAddr() ) {
	                //debugEV << "Node " << macPkt->getSrcAddr().getInt() << " claims about node " <<  idsResponsePkt->getNodeID() << " that it is attacker: " << idsResponsePkt->getIsAttacker()  << endl;
	                // The claim is recorded for later evaluation
	                if (idsResponsePkt->getSrcAddr() != idsResponsePkt->getNodeID()) {
	                    recordResponse(idsResponsePkt);
	                } else {
	                    debugEV << "We do not accept claim from suspicious node." << endl;
	                }
	            } else {
	                debugEV << "The IDS voting response packet is not for us." << endl;
	                //debugEV << "Destination: " << idsResponsePkt->getDestAddr() << ", sender: " <<  macPkt->getSrcAddr().getInt() << endl;
	            }
	            delete msg;
	            return;
	        }
	    }
	}

	IDSLayer::handleLowerMsg(msg);
}

/**
 * Handle messages from itself - here, the time windows are managed
 */
void IDSSimpleLayer::handleSelfMsg(cMessage *msg) {
    if (msg->getKind() == IDS_WINDOW_TIMER) {
        debugEV << "New window is being started...." << endl;
        debugEV << msg->getName() << endl;

        // Here the past window is being evaluated:
        evaluateLastWindow();

        // And the last records should be deleted:
        resetWindowValues();

        scheduleAt(simTime() + windowSize, idsWindowTimer);
    } else if (msg->getKind() == IDS_RESPONSES_COLLECTION_TIMER) {
        debugEV << "It is time to evaluate responses about node " << msg->getName() << endl;
        evaluateResponses(atoi(msg->getName()) );
        delete msg;
    } else {
        delete msg;
    }


}

/**
 * Finalize module
 */
void IDSSimpleLayer::finish(){
	BaseLayer::finish();
	//LOCAL EVALUATION CURRENTLY TURNED ON!
	doEvaluation();

	if (!par("stats").boolValue()) return;

	// compute various statistics
	cOutVector neighborsVector("neighbors");
	for (std::set<int>::iterator it=neighborsSet.begin(); it!=neighborsSet.end(); it++)
		neighborsVector.record((*it));

	cOutVector forwardersVector("forwardersMap");
	for (IDSMap::iterator it=forwardersMap.begin();it!=forwardersMap.end();it++)
		forwardersVector.record(it->first);

	if (fullStats) {
	    recordScalar("data_packets_intercepted", numInterceptedPackets);
	    recordScalar("data_packets_me_to_somebody", numMe2Somebody);
	    recordScalar("data_packets_somebody_to_me", numSomebody2Me);
	    recordScalar("data_packets_somebody_to_somebody", numSomebody2Somebody);
	}

	/*recordScalar("neighbors", neighborsSet.size());
	recordScalar("forwarders_falseNegatives", forwardersStats.falseNegatives);
	recordScalar("forwarders_falsePositives", forwardersStats.falsePositives);
	recordScalar("forwarders_trueNegatives", forwardersStats.trueNegatives);
	recordScalar("forwarders_truePositives", forwardersStats.truePositives);
	*/
}

/**
 * Analyse received network packet
 */
void IDSSimpleLayer::analyseNetwPkt(StaticNetwPkt* netwPkt){
	// packet is OK, analyse it
	if (netwPkt != NULL){
//
//		// compute stats
		numInterceptedPackets++;
		if (netwPkt->getSrcAddr()==net->getNetwAddr()){
			numMe2Somebody++;
		} else {
			if (netwPkt->getDestAddr()==net->getNetwAddr()) numSomebody2Me++;
			else numSomebody2Somebody++;
		}
//
//		// write info about received packet
		debugEV << "IDS on "<< net->getNetwAddr() << " received packet:"
		<< " src=" << netwPkt->getSrcAddr() << " (" << helper->getNodeDistance(net->getNetwAddr(), netwPkt->getSrcAddr()) << " m)"
		<< " dest=" << netwPkt->getDestAddr() << " (" << helper->getNodeDistance(net->getNetwAddr(), netwPkt->getDestAddr()) << " m)"
		<< " initialSrc=" << netwPkt->getInitialSrcAddr()
		<< " finalDest=" << netwPkt->getFinalDestAddr()
		<< " seqNum=" << netwPkt->getSeqNum()
		<< " nbHops=" << netwPkt->getNbHops()
		<< " creationTime=" << netwPkt->getCreationTime().str()
		<< endl;

		// add to neighborsSet
		if (netwPkt->getSrcAddr()!=net->getNetwAddr() && neighborsSet.count(netwPkt->getSrcAddr())==0){
			if (maxNeighbor==-1 || helper->getNodeDistance(net->getNetwAddr(), netwPkt->getSrcAddr()) > helper->getNodeDistance(net->getNetwAddr(), maxNeighbor))
				maxNeighbor = netwPkt->getSrcAddr();
			neighborsSet.insert(netwPkt->getSrcAddr());
			// Martin
			debugEV << "Inserted new node, current size of the neighbors set is: " << neighborsSet.size() << endl;
			debugEV << "Inserted new node, current size of the forwarders map is: " << forwardersMap.size() << endl;
		}

		double dist2dest = helper->getNodeDistance(net->getNetwAddr(), netwPkt->getDestAddr());

		// packet is being forwarded and we are monitoring the source node
		if (netwPkt->getSrcAddr() != netwPkt->getInitialSrcAddr() &&
				netwPkt->getSrcAddr() != net->getNetwAddr() &&
				helper->getNodeDistance(net->getNetwAddr(), netwPkt->getSrcAddr()) <= maxDistance){

			if (forwardersMap.count(netwPkt->getSrcAddr())>0){
				debugEV<< "Node " << netwPkt->getSrcAddr() << " forwarded this packet " << endl;

				for(FwdBuffer::iterator entry=fwdBuffer.begin(); entry!=fwdBuffer.end(); entry++){
					if (entry->destAddr == netwPkt->getFinalDestAddr() &&
							entry->srcAddr == netwPkt->getInitialSrcAddr() &&
							entry->creationTime == netwPkt->getCreationTime().raw() &&
							entry->nodeAddr == netwPkt->getSrcAddr()){

						forwardersMap[netwPkt->getSrcAddr()].packetsForwarded++;

						// increment last window forwarding
						forwardersMap[netwPkt->getSrcAddr()].packetsLastWindowForwarded++;

						debugEV<< "Packet removed from fwdBuffer" << endl;

						fwdBuffer.erase(entry);
						break;
					}
				}
			}
		}

		// packet should be forwarded and we are monitoring the destination node (is close enough and our neighbor or routing tree parent)
		if (netwPkt->getDestAddr() != netwPkt->getFinalDestAddr() &&
				netwPkt->getDestAddr() != net->getNetwAddr() &&
				dist2dest <= maxDistance &&
				(neighborsSet.count(netwPkt->getDestAddr()) > 0 || net->getNextHopAddr() == netwPkt->getDestAddr())){

			bool monitor = true;

			// we need to throw something out from forwardersMap
			if (forwardersMap.count(netwPkt->getDestAddr())==0 && forwardersMap.size()>=maxMonitoredNodes){
				// find node with maximum distance so far
				double foundDist = 0;
				int foundNode = -1;

				for(IDSMap::iterator entry=forwardersMap.begin(); entry!=forwardersMap.end(); entry++){
					double dist = helper->getNodeDistance(net->getNetwAddr(), entry->first);
					// possible candidate only if distant enough and not my parent in routing tree
					if (dist > foundDist && net->getNextHopAddr() != entry->first){
						foundDist = dist;
						foundNode = entry->first;
					}
				}

				// if there is worse entry in the map, remove it and insert new one
				if (foundDist > dist2dest || net->getNextHopAddr() == netwPkt->getDestAddr()){
					debugEV<< "Removing node " << foundNode << " from forwardersMap" << endl;
					forwardersMap.erase(foundNode);

					// remove all packets from fwdBuffer
					for(FwdBuffer::iterator entry=fwdBuffer.begin(); entry!=fwdBuffer.end(); ){
						if (entry->nodeAddr == netwPkt->getDestAddr())
							entry = fwdBuffer.erase(entry);
						else
							entry++;
					}
				}

				// Is it possible to monitor given node?
				if (forwardersMap.count(netwPkt->getDestAddr())==0 && forwardersMap.size()>=maxMonitoredNodes)
				    monitor = false;

			}

			// if the packet is being monitored
			if (monitor){
				// update buffer and create one empty slot
				updateFwdBuffer(true);

				if (fwdBuffer.size()<fwdBufferSize){
					FwdEntry fwdEntry;
					fwdEntry.timeout = simTime() + SimTime(fwdPacketTimeout);
					fwdEntry.srcAddr = netwPkt->getInitialSrcAddr();
					fwdEntry.destAddr = netwPkt->getFinalDestAddr();
					fwdEntry.nodeAddr = netwPkt->getDestAddr();
					fwdEntry.creationTime = netwPkt->getCreationTime().raw();
					fwdBuffer.push_back(fwdEntry);

					// increment PR
					forwardersMap[netwPkt->getDestAddr()].packetsReceived++;

					// increment window PR
					forwardersMap[netwPkt->getDestAddr()].packetsLastWindowReceived++;

					// log
					debugEV<< "Node " << netwPkt->getDestAddr() << " should forward packet " << endl;
					debugEV<< "Node " << netwPkt->getDestAddr() << " already received " <<  forwardersMap[netwPkt->getDestAddr()].packetsReceived << " packets." << endl;
				}
			}
		}
	} else {
		debugEV << "Node " << net->getNetwAddr() << " received unknown packet" << endl;
	}
}


/**
 * Updates buffer of packets, that need to be forwarded
 */
void IDSSimpleLayer::updateFwdBuffer(bool needOneSlot){
	// check timeouts and throw away old packets
	for(FwdBuffer::iterator entry=fwdBuffer.begin(); entry!=fwdBuffer.end(); ){
		if (simTime() >= entry->timeout){
			debugEV << "Packet is old, deleting (" << entry->srcAddr << "->" << entry->destAddr << ")" << endl;
			debugEV << "SimTime=" << simTime() << ", timeout=" << entry->timeout << endl;
			entry = fwdBuffer.erase(entry);
		} else {
			entry++;
		}
	}
	// we need one empty slot and buffer is full => delete oldest packet
	if (needOneSlot && fwdBuffer.size()>0 && fwdBuffer.size()>=fwdBufferSize){
		FwdEntry* entry = &fwdBuffer.front();
		debugEV << "Buffer is full, deleting packet (" << entry->srcAddr << "->" << entry->destAddr << ")" << endl;
		fwdBuffer.pop_front();
	}
}
/**
 * Perform IDS evaluation and compute stats
 */
void IDSSimpleLayer::doEvaluation(){
	if (evaluated) return;

	forwardersStats.reset();

	cModule* network = FindModule<>::findNetwork(this);
	cModule* node;
	StaticNetwLayer* netw;

	// check forwarders
	for (IDSMap::iterator i=forwardersMap.begin();i!=forwardersMap.end();i++){
		IDSEntry* entry = &(i->second);
		const int nodeAddr = i->first;
		stringstream ss1;
		if (fullStats) {
            ss1 << "neigh"<<nodeAddr<< "PackRec";
                recordScalar(ss1.str().c_str(),entry->packetsReceived);
            ss1.str("");
            ss1 << "neigh"<<nodeAddr<< "PackFwd";
                recordScalar(ss1.str().c_str(),entry->packetsForwarded);
            ss1.str("");
		}
        ss1 << "neigh"<<nodeAddr<< "DropLoc";
            recordScalar(ss1.str().c_str(),entry->isDropperLocal);
        ss1.str("");
        ss1 << "neigh"<<nodeAddr<< "DropGlob";
            recordScalar(ss1.str().c_str(),entry->isDropperGlobal);
        ss1.str("");
		// get node
        if (fullStats) {
            node = network->getSubmodule("node", nodeAddr);
            netw = FindModule<StaticNetwLayer*>::findSubModule(node);
            if (!netw) continue;
            ss1 << "neighbour"<<nodeAddr<< "IntentionalPacketDropping";
                recordScalar(ss1.str().c_str(),netw->pPacketDropping);
        }
	}
	// Martin
	if (fullStats) {
	    recordScalar("Neigbors_forwardersMap", forwardersMap.size());
	    recordScalar("Neighbors_neighborsSet", neighborsSet.size());
	}

	//evaluated = true;
}

/**
 * Evaluate IDS statistics over the last IDS window
 */
void IDSSimpleLayer::evaluateLastWindow() {

    // Our window is over -> response to pending requests
    responseVotingRequests();

    // The evaluation should be done for all monitored neighbors
    debugEV << "Going to evaluate all monitored neighbors, the number of them is " << forwardersMap.size() << endl;
    for (IDSMap::iterator i=forwardersMap.begin();i!=forwardersMap.end();i++){
        IDSEntry* entry = &(i->second);
        const int nodeAddr = i->first;
        debugEV << "Node " << nodeAddr << " received " << entry->packetsLastWindowReceived << " and forwarded " << entry->packetsLastWindowForwarded << " packets." << endl;
        debugEV << "Node " << nodeAddr << " is locally considered dropper: " << entry->isDropperLocal << endl;
        debugEV << "Node " << nodeAddr << " is globally considered dropper: " << entry->isDropperGlobal << endl;

        // If dropping over the last window was higher than threshold, start voting scheme:
        if (entry->getPacketsLastWindowDroppedRatio() > fwdDetectionThreshold) {
            debugEV << "Node " << nodeAddr << " dropped " << entry->getPacketsLastWindowDroppedRatio()*100 <<
                    " % packets. There is an assumption that it is a dropper => voting scheme?" << endl;
            // Since now we locally assume this node as dropper:
            entry->isDropperLocal = true;
            // Start the voting scheme if not yet considered dropper:
            if (!entry->isDropperGlobal) {
                debugEV << "Node is not globally decided as dropper yet. Voting." << endl;
                askNeighborsForDropping(nodeAddr);
            } else {
                debugEV << "Node is already globally decided as dropper. No voting." << endl;
            }
        }

    }

}

/**
 * Here the pending request are responded
 */
void IDSSimpleLayer::responseVotingRequests() {

    // Sending responses to the pending requests:
    while (!responseBuffer.empty()) {

        debugEV << "Response buffer size: " << responseBuffer.size() << endl;
        debugEV << "IDS node: " << responseBuffer.front().srcAddr << " asks on " << responseBuffer.front().suspiciousAddr << endl;

        // Check whether we monitor corresponding node and whether we received enough packets from it.
        if (forwardersMap.count(responseBuffer.front().suspiciousAddr)>0 && forwardersMap[responseBuffer.front().suspiciousAddr].packetsLastWindowReceived > fwdMinPacketsReceived) {

            debugEV << "Sending response to: " << responseBuffer.front().srcAddr << " on node " << responseBuffer.front().suspiciousAddr << endl;

            IDSVotingResponsePkt *idsResponsePkt = new IDSVotingResponsePkt();

            idsResponsePkt->setDestAddr(responseBuffer.front().srcAddr);
            idsResponsePkt->setSrcAddr(net->getNetwAddr());
            idsResponsePkt->setNodeID(responseBuffer.front().suspiciousAddr);
            idsResponsePkt->setIsAttacker(forwardersMap[responseBuffer.front().suspiciousAddr].isDropperLocal);

            NetwControlInfo::setControlInfo(idsResponsePkt, idsResponsePkt->getDestAddr());

            // Is this correct if many packets are about to be send??? Should be managed by MAC layer.
            send(idsResponsePkt, idsSenderControlOut);
        }
        responseBuffer.pop();
     }
}


/**
 * There is an assumption that node is dropper -> here the neighbors are being asked for their claim
 */
void IDSSimpleLayer::askNeighborsForDropping(int nodeAddr) {

    if (forwardersMap[nodeAddr].isWaitingForResponses == true) {
        debugEV << "Already waiting for responses from another window" << endl;
        return;
    }

    debugEV << "Node " << nodeAddr << " is assumed to be an attacker => ask neighbors " << endl;

    // Here we have to send a packet to ask neighbors for decision about the node -> this is done by IDS sender
    IDSVotingPkt *idsPkt = new IDSVotingPkt();

    idsPkt->setDestAddr(LAddress::L3BROADCAST);
    idsPkt->setSrcAddr(net->getNetwAddr());
    idsPkt->setNodeID(nodeAddr);
    NetwControlInfo::setControlInfo(idsPkt, idsPkt->getDestAddr());

    send(idsPkt, idsSenderControlOut);

    // Start the timer for responses collection:
//    idsResponsesCollectionTimer = new cMessage("" + nodeAddr, IDS_RESPONSES_COLLECTION_TIMER);
    char kind[5];
    sprintf(kind, "%d", nodeAddr);
    idsResponsesCollectionTimer = new cMessage(kind, IDS_RESPONSES_COLLECTION_TIMER);
    scheduleAt(simTime() + collectingTime, idsResponsesCollectionTimer);

    forwardersMap[nodeAddr].isWaitingForResponses = true;

}

void IDSSimpleLayer::resetWindowValues() {
    debugEV << "Window values are being reset." << endl;

    for (IDSMap::iterator i=forwardersMap.begin();i!=forwardersMap.end();i++){
        IDSEntry* entry = &(i->second);
        const int nodeAddr = i->first;
        debugEV << "Window records of node " << nodeAddr << " are reset." << endl;
        entry->packetsLastWindowForwarded = 0;
        entry->packetsLastWindowReceived = 0;

    }
}

void IDSSimpleLayer::votingResponse(IDSVotingPkt* idsPkt) {

    // Pending requirements are buffered and sent once our window is over.
    responseBuffer.push(ResponseEntry(idsPkt->getSrcAddr(), idsPkt->getNodeID()));

    debugEV << "Size of the response buffer is " << responseBuffer.size() << endl;

    return;
}

void IDSSimpleLayer::recordResponse(IDSVotingResponsePkt* idsResponsePkt) {
    if (idsResponsePkt->getIsAttacker()) {
        forwardersMap[idsResponsePkt->getNodeID()].positiveResponses++;
    } else {
        forwardersMap[idsResponsePkt->getNodeID()].negativeResponses++;
    }
    debugEV << "IDS Responses about node " << idsResponsePkt->getNodeID() << ": " <<
            forwardersMap[idsResponsePkt->getNodeID()].positiveResponses << " positive responses and " <<
            forwardersMap[idsResponsePkt->getNodeID()].negativeResponses << " negative responses." << endl;
}

/**
 * This is used to evaluate responses about node when the collection time is over
 */
void IDSSimpleLayer::evaluateResponses(int nodeID) {
    debugEV << "We are going to evaluate responses about node " << nodeID << endl;
    debugEV << "IDS Responses about node " << nodeID << ": " <<
                forwardersMap[nodeID].positiveResponses << " positive responses and " <<
                forwardersMap[nodeID].negativeResponses << " negative responses." << endl;

    // In case of no response
    if (forwardersMap[nodeID].positiveResponses == 0 && forwardersMap[nodeID].negativeResponses == 0) {
        forwardersMap[nodeID].isWaitingForResponses = false;
        return;
    }

    if ( ( ( forwardersMap[nodeID].positiveResponses /
        (forwardersMap[nodeID].positiveResponses + forwardersMap[nodeID].negativeResponses) ) >
        votingThreshold ) && ( (forwardersMap[nodeID].positiveResponses + forwardersMap[nodeID].negativeResponses) >= minVotesReceived) ) {
        debugEV << "Since the ratio of positive responses is higher than threshold and we received enough responses, the node " << nodeID << " will be considered dropper globally" << endl;
        forwardersMap[nodeID].isDropperGlobal = true;
    }

    // Reset the responses for next time.
    forwardersMap[nodeID].positiveResponses = 0;
    forwardersMap[nodeID].negativeResponses = 0;

    forwardersMap[nodeID].isWaitingForResponses = false;
}

IDSSimpleLayer::~IDSSimpleLayer()
{
    cancelAndDelete(idsWindowTimer);
}
