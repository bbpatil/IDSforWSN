cplusplus {{
#include "NetwPkt_m.h"
#include "MessageTypes.h" 
}}

//class NetwPkt;
class LAddress::L3Type extends void;
class noncobject Attack_t;

packet IDSVotingResponsePkt
{
    LAddress::L3Type destAddr;   // destination address
    LAddress::L3Type srcAddr;    // source address
	int nodeID;
	bool isAttacker;
	Attack_t           attackKind;
}
