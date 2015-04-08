cplusplus {{
#include "NetwPkt_m.h"
}}

//class NetwPkt;
class LAddress::L3Type extends void;

packet IDSVotingResponsePkt
{
    LAddress::L3Type destAddr;   // destination address
    LAddress::L3Type srcAddr;    // source address
	int nodeID;
	bool isAttacker;
}
