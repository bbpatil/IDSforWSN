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

#ifndef IDSSIMPLELAYER_H_
#define IDSSIMPLELAYER_H_

#include <omnetpp.h>
#include <map>
#include <vector>
#include <deque>
#include <set>
#include <queue>
#include <BMacLayer.h>
#include <MacPkt_m.h>
#include <ApplPkt_m.h>

#include "IDSLayer.h"
#include "StaticNetwLayer.h"
#include "StaticNetwPkt_m.h"
#include "SimHelper.h"
#include "IDSVotingPkt_m.h"
#include "IDSVotingResponsePkt_m.h"


/**
 * Entry in IDS table
 */
struct IDSEntry {
    // This is to filter out retransmitted packets - TODO
    // TODO: How will we deal with packets that are retransmitted? Because we should use MAC Acknowledgements. But if a packet is retransmitted,
    // we should not probably buffer all retransmissions...
    int64 lastCreationTime;
    int lastSrcAddr;

	unsigned int packetsReceived, packetsForwarded;

	unsigned int packetsLastWindowReceived, packetsLastWindowForwarded;

	unsigned int positiveResponses, negativeResponses;

	// This is to make own claim about a node:
	bool isDropperLocal, isDropperGlobal;

	bool isWaitingForResponses;

	// Values for delay dropper
	unsigned int delayedPacketsLastWindow, notDelayedPacketsLastWindow;

	// This is to make own claim about a node's delay:
	bool isDelayerLocal, isDelayerGlobal;

	IDSEntry(){
		packetsReceived = packetsForwarded = packetsLastWindowReceived = packetsLastWindowForwarded = lastCreationTime = lastSrcAddr = 0;
		positiveResponses = negativeResponses = 0;
		isDropperLocal = isDropperGlobal = false;
		isWaitingForResponses = false;
		delayedPacketsLastWindow = notDelayedPacketsLastWindow = 0;
		isDelayerLocal = isDelayerGlobal = false;
	}
	double getPacketsDroppedRatio(){
		return packetsReceived>0?(packetsReceived-packetsForwarded)/(double)packetsReceived:0;
	}

    double getPacketsLastWindowDroppedRatio(){
        return packetsLastWindowReceived>0?(packetsLastWindowReceived-packetsLastWindowForwarded)/(double)packetsLastWindowReceived:0;
    }

    double getDelayLastWindowDroppedRatio(){
        return packetsLastWindowReceived>0?(delayedPacketsLastWindow)/(double)(delayedPacketsLastWindow+notDelayedPacketsLastWindow):0;
    }
};
typedef std::map<int, IDSEntry> IDSMap;

/**
 * Entry in forwarders table
 */
struct FwdEntry {
	int srcAddr;
	int destAddr;
	int nodeAddr;
	int64 creationTime;

	simtime_t delayTime;

	simtime_t timeout;

	bool markedAsDelayed;

	FwdEntry(){
		srcAddr = destAddr = nodeAddr = creationTime = -1;
		markedAsDelayed = false;
	}
};

/**
 * Entry in response buffer
 */
struct ResponseEntry {
    int srcAddr;
    int suspiciousAddr;

    ResponseEntry(){
        srcAddr = suspiciousAddr = 0;
    }

    ResponseEntry(int x, int y){
        srcAddr = x;
        suspiciousAddr = y;
    }
};

typedef std::pair<int, int64> FwdMapIndex;
typedef std::map<FwdMapIndex, FwdEntry> FwdMap;
typedef std::deque<FwdEntry> FwdBuffer;

typedef std::queue<ResponseEntry> IDSResponseBuffer;

/**
 * IDS statistics
 */
typedef struct _IDSStats {
	unsigned long falsePositives;
	unsigned long falseNegatives;
	unsigned long truePositives;
	unsigned long trueNegatives;

	_IDSStats(){
		reset();
	}
	void reset(){
		falsePositives = falseNegatives = truePositives = trueNegatives = 0;
	}
} IDSStats;


class IDSSimpleLayer : public IDSLayer {

protected:

    enum messagesTypes {
        IDS_WINDOW_TIMER=0,
        IDS_RESPONSES_COLLECTION_TIMER=1
    };

	bool evaluated;
	double maxDistance;
	unsigned int maxMonitoredNodes;
	double monitoringTime;
	bool fullStats;

	SimHelper* helper;
	StaticNetwLayer* net;

	FwdBuffer fwdBuffer;

	virtual void handleUpperMsg(cMessage *msg);
	virtual void handleLowerMsg(cMessage *msg);
	virtual void handleSelfMsg(cMessage *msg);

	void analyseNetwPkt(StaticNetwPkt* netwPkt);
	void updateFwdBuffer(bool needOneSlot=false);

	cMessage* idsWindowTimer;
	cMessage* idsResponsesCollectionTimer;

	// Gate for communication with IDS sender
	int idsSenderControlOut;

public:
	virtual void finish();
	virtual void initialize(int stage);
	void doEvaluation();
	virtual ~IDSSimpleLayer();
	void evaluateLastWindow();
	void resetWindowValues();
	void askNeighborsForDropping(int nodeID, bool delayAttack);
	void votingResponse(IDSVotingPkt* idsPkt);
	void recordResponse(IDSVotingResponsePkt* idsResponsePkt);
	void responseVotingRequests();
	void evaluateResponses(int nodeID);

	IDSStats forwardersStats;
	IDSMap forwardersMap;
	IDSResponseBuffer responseBuffer;
	std::set<int> neighborsSet;
	double maxNeighbor;
	unsigned long int usedMemory;
	unsigned int numInterceptedPackets;
	unsigned int numSomebody2Me, numSomebody2Somebody, numMe2Somebody;

	unsigned int fwdBufferSize, fwdMinPacketsReceived;
	double fwdPacketTimeout, fwdDetectionThreshold;
	unsigned int collectingTime;

	// New parameters:
	unsigned int windowSize;
	unsigned int minVotesReceived;
	double votingThreshold;

	// Delay attack parameters:
	bool enabledDelayDetection;
    // P8: how long time a packet has to be delayed to be marked as "delayed"
    double delayTime;
    // P9: how many packets have to be delayed in a time window to be consider as "Delay attacker"
    double delayThreshold;
    // P10: Size of a time window to evaluate delays -> not used now
    // double delayWindowSize;

};


#endif /* IDSSIMPLELAYER_H_ */
