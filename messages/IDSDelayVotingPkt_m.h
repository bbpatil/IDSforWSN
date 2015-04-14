//
// Generated file, do not edit! Created by opp_msgc 4.3 from messages/IDSDelayVotingPkt.msg.
//

#ifndef _IDSDELAYVOTINGPKT_M_H_
#define _IDSDELAYVOTINGPKT_M_H_

#include <omnetpp.h>

// opp_msgc version check
#define MSGC_VERSION 0x0403
#if (MSGC_VERSION!=OMNETPP_VERSION)
#    error Version mismatch! Probably this file was generated by an earlier version of opp_msgc: 'make clean' should help.
#endif

// cplusplus {{
#include "NetwPkt_m.h"
// }}



/**
 * Class generated from <tt>messages/IDSDelayVotingPkt.msg</tt> by opp_msgc.
 * <pre>
 * packet IDSDelayVotingPkt
 * {
 *     LAddress::L3Type destAddr;   
 *     LAddress::L3Type srcAddr;    
 * 	int              nodeID;
 * 	bool             delayAttack;
 * }
 * </pre>
 */
class IDSDelayVotingPkt : public ::cPacket
{
  protected:
    LAddress::L3Type destAddr_var;
    LAddress::L3Type srcAddr_var;
    int nodeID_var;
    bool delayAttack_var;

  private:
    void copy(const IDSDelayVotingPkt& other);

  protected:
    // protected and unimplemented operator==(), to prevent accidental usage
    bool operator==(const IDSDelayVotingPkt&);

  public:
    IDSDelayVotingPkt(const char *name=NULL, int kind=0);
    IDSDelayVotingPkt(const IDSDelayVotingPkt& other);
    virtual ~IDSDelayVotingPkt();
    IDSDelayVotingPkt& operator=(const IDSDelayVotingPkt& other);
    virtual IDSDelayVotingPkt *dup() const {return new IDSDelayVotingPkt(*this);}
    virtual void parsimPack(cCommBuffer *b);
    virtual void parsimUnpack(cCommBuffer *b);

    // field getter/setter methods
    virtual LAddress::L3Type& getDestAddr();
    virtual const LAddress::L3Type& getDestAddr() const {return const_cast<IDSDelayVotingPkt*>(this)->getDestAddr();}
    virtual void setDestAddr(const LAddress::L3Type& destAddr);
    virtual LAddress::L3Type& getSrcAddr();
    virtual const LAddress::L3Type& getSrcAddr() const {return const_cast<IDSDelayVotingPkt*>(this)->getSrcAddr();}
    virtual void setSrcAddr(const LAddress::L3Type& srcAddr);
    virtual int getNodeID() const;
    virtual void setNodeID(int nodeID);
    virtual bool getDelayAttack() const;
    virtual void setDelayAttack(bool delayAttack);
};

inline void doPacking(cCommBuffer *b, IDSDelayVotingPkt& obj) {obj.parsimPack(b);}
inline void doUnpacking(cCommBuffer *b, IDSDelayVotingPkt& obj) {obj.parsimUnpack(b);}


#endif // _IDSDELAYVOTINGPKT_M_H_