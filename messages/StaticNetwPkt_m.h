//
// Generated file, do not edit! Created by opp_msgc 4.3 from messages/StaticNetwPkt.msg.
//

#ifndef _STATICNETWPKT_M_H_
#define _STATICNETWPKT_M_H_

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
 * Class generated from <tt>messages/StaticNetwPkt.msg</tt> by opp_msgc.
 * <pre>
 * packet StaticNetwPkt extends NetwPkt {
 * 	LAddress::L3Type finalDestAddr;   
 *  	LAddress::L3Type initialSrcAddr;
 *  	int              nbHops;
 * }
 * </pre>
 */
class StaticNetwPkt : public ::NetwPkt
{
  protected:
    LAddress::L3Type finalDestAddr_var;
    LAddress::L3Type initialSrcAddr_var;
    int nbHops_var;

  private:
    void copy(const StaticNetwPkt& other);

  protected:
    // protected and unimplemented operator==(), to prevent accidental usage
    bool operator==(const StaticNetwPkt&);

  public:
    StaticNetwPkt(const char *name=NULL, int kind=0);
    StaticNetwPkt(const StaticNetwPkt& other);
    virtual ~StaticNetwPkt();
    StaticNetwPkt& operator=(const StaticNetwPkt& other);
    virtual StaticNetwPkt *dup() const {return new StaticNetwPkt(*this);}
    virtual void parsimPack(cCommBuffer *b);
    virtual void parsimUnpack(cCommBuffer *b);

    // field getter/setter methods
    virtual LAddress::L3Type& getFinalDestAddr();
    virtual const LAddress::L3Type& getFinalDestAddr() const {return const_cast<StaticNetwPkt*>(this)->getFinalDestAddr();}
    virtual void setFinalDestAddr(const LAddress::L3Type& finalDestAddr);
    virtual LAddress::L3Type& getInitialSrcAddr();
    virtual const LAddress::L3Type& getInitialSrcAddr() const {return const_cast<StaticNetwPkt*>(this)->getInitialSrcAddr();}
    virtual void setInitialSrcAddr(const LAddress::L3Type& initialSrcAddr);
    virtual int getNbHops() const;
    virtual void setNbHops(int nbHops);
};

inline void doPacking(cCommBuffer *b, StaticNetwPkt& obj) {obj.parsimPack(b);}
inline void doUnpacking(cCommBuffer *b, StaticNetwPkt& obj) {obj.parsimUnpack(b);}


#endif // _STATICNETWPKT_M_H_
