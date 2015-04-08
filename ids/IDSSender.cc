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

#include "IDSSender.h"
#include "ArpInterface.h"
#include "FindModule.h"

#include "IDSVotingPkt_m.h"

Define_Module(IDSSender)

/**
 * Initialize module
 */
void IDSSender::initialize(int stage){
//    BaseNetwLayer::initialize(stage);
    BaseLayer::initialize(stage);
    if (stage==0){
        // TODO: check the initialization against BaseNetwLayer !!!
        arp = FindModule<ArpInterface*>::findSubModule(findHost());
        idsControlIn = findGate("idsControlIn");
        // Start the timer for window size:
//        idsSenderTimer = new cMessage("ids-sender-timer", 0);
//        scheduleAt(simTime() + 1, idsSenderTimer);
    }
}

void IDSSender::handleMessage(cMessage* msg) {
    // Martin
//    EV << "gate id is " << msg->getArrivalGateId() << endl;
//    EV << "upperLayerIn id is " << upperLayerIn << endl;
//    EV << "upperControlIn id is " << upperControlIn << endl;
//    EV << "lowerControlIn id is " << lowerControlIn << endl;
//    EV << "lowerLayerIn id is " << lowerLayerIn << endl;
    if (msg->isSelfMessage()) {
        handleSelfMsg(msg);
    } else if(msg->getArrivalGateId()==upperLayerIn) {
        recordPacket(PassedMessage::INCOMING,PassedMessage::UPPER_DATA,msg);
        handleUpperMsg(msg);
    } else if(msg->getArrivalGateId()==upperControlIn) {
        recordPacket(PassedMessage::INCOMING,PassedMessage::UPPER_CONTROL,msg);
        handleUpperControl(msg);
    } else if(msg->getArrivalGateId()==lowerControlIn){
        recordPacket(PassedMessage::INCOMING,PassedMessage::LOWER_CONTROL,msg);
        handleLowerControl(msg);
    } else if(msg->getArrivalGateId()==lowerLayerIn) {
        recordPacket(PassedMessage::INCOMING,PassedMessage::LOWER_DATA,msg);
        handleLowerMsg(msg);
    // This is added to standard "handleMessage" function:
    } else if(msg->getArrivalGateId()==idsControlIn) {
        recordPacket(PassedMessage::INCOMING,PassedMessage::LOWER_CONTROL,msg);
        handleIDSControl(msg);
    }
    else if(msg->getArrivalGateId()==-1) {
        /* Classes extending this class may not use all the gates, f.e.
         * BaseApplLayer has no upper gates. In this case all upper gate-
         * handles are initialized to -1. When getArrivalGateId() equals -1,
         * it would be wrong to forward the message to one of these gates,
         * as they actually don't exist, so raise an error instead.
         */
        opp_error("No self message and no gateID?? Check configuration.");
    } else {
        /* msg->getArrivalGateId() should be valid, but it isn't recognized
         * here. This could signal the case that this class is extended
         * with extra gates, but handleMessage() isn't overridden to
         * check for the new gate(s).
         */
        opp_error("Unknown gateID?? Check configuration or override handleMessage().");
    }
}

/**
 * Handle self messages
 */
void IDSSender::handleSelfMsg(cMessage* msg){
    switch(msg->getKind()) {
    default:
        EV << "IDS Sender: Unknown selfmessage! -> delete, kind: "<<msg->getKind() <<endl;
        delete msg;
        break;
    }
}

/**
 * Handle messages from upper layer
 */
void IDSSender::handleUpperMsg(cMessage *msg){
    sendDown(msg);
}

/**
 * Handle messages from lower layer
 */
void IDSSender::handleLowerMsg(cMessage *msg){
    sendUp(msg);
}

/**
 * Handle control messages from lower layer
 */
void IDSSender::handleLowerControl(cMessage *msg){
    sendControlUp(msg);
}

/**
 * Handle control messages from upper layer
 */
void IDSSender::handleUpperControl(cMessage *msg){
    sendControlDown(msg);
}

void IDSSender::handleIDSControl(cMessage *msg) {
    debugEV << "An IDS control message has arrived" << endl;

//    assert(dynamic_cast<cPacket*>(msg));
//    sendDown(encapsMsg(static_cast<cPacket*>(msg)));
    BaseNetwLayer::handleUpperMsg(msg);

    //    IDSVotingPkt *idsPkt = IDSVotingPkt(msg);
//    sendDown(idsPkt);
}

//IDSSender::IDSSender() {
//    // TODO Auto-generated constructor stub
//
//}
//
//IDSSender::~IDSSender() {
//    // TODO Auto-generated destructor stub
//}

