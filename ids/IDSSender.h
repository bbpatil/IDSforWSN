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

#ifndef IDSSENDER_H_
#define IDSSENDER_H_

#include <omnetpp.h>
#include <BaseNetwLayer.h>

class IDSSender : public BaseNetwLayer
{
protected:
    cMessage* idsSenderTimer;

    int idsControlIn;

public:
    /** @brief Called every time a message arrives*/
    virtual void handleMessage( cMessage* );

    virtual void initialize(int stage);
    virtual void handleSelfMsg(cMessage* msg);
    virtual void handleUpperMsg(cMessage *msg);
    virtual void handleLowerMsg(cMessage *msg);
    virtual void handleLowerControl(cMessage *msg);
    virtual void handleUpperControl(cMessage *msg);
    virtual void handleIDSControl(cMessage *msg);
};

#endif /* IDSSENDER_H_ */
