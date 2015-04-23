//
// Generated file, do not edit! Created by opp_msgc 4.3 from messages/IDSVotingResponsePkt.msg.
//

// Disable warnings about unused variables, empty switch stmts, etc:
#ifdef _MSC_VER
#  pragma warning(disable:4101)
#  pragma warning(disable:4065)
#endif

#include <iostream>
#include <sstream>
#include "IDSVotingResponsePkt_m.h"

// Template rule which fires if a struct or class doesn't have operator<<
template<typename T>
std::ostream& operator<<(std::ostream& out,const T&) {return out;}

// Another default rule (prevents compiler from choosing base class' doPacking())
template<typename T>
void doPacking(cCommBuffer *, T& t) {
    throw cRuntimeError("Parsim error: no doPacking() function for type %s or its base class (check .msg and _m.cc/h files!)",opp_typename(typeid(t)));
}

template<typename T>
void doUnpacking(cCommBuffer *, T& t) {
    throw cRuntimeError("Parsim error: no doUnpacking() function for type %s or its base class (check .msg and _m.cc/h files!)",opp_typename(typeid(t)));
}




Register_Class(IDSVotingResponsePkt);

IDSVotingResponsePkt::IDSVotingResponsePkt(const char *name, int kind) : cPacket(name,kind)
{
    this->nodeID_var = 0;
    this->isAttacker_var = 0;
}

IDSVotingResponsePkt::IDSVotingResponsePkt(const IDSVotingResponsePkt& other) : cPacket(other)
{
    copy(other);
}

IDSVotingResponsePkt::~IDSVotingResponsePkt()
{
}

IDSVotingResponsePkt& IDSVotingResponsePkt::operator=(const IDSVotingResponsePkt& other)
{
    if (this==&other) return *this;
    cPacket::operator=(other);
    copy(other);
    return *this;
}

void IDSVotingResponsePkt::copy(const IDSVotingResponsePkt& other)
{
    this->destAddr_var = other.destAddr_var;
    this->srcAddr_var = other.srcAddr_var;
    this->nodeID_var = other.nodeID_var;
    this->isAttacker_var = other.isAttacker_var;
    this->attackKind_var = other.attackKind_var;
}

void IDSVotingResponsePkt::parsimPack(cCommBuffer *b)
{
    cPacket::parsimPack(b);
    doPacking(b,this->destAddr_var);
    doPacking(b,this->srcAddr_var);
    doPacking(b,this->nodeID_var);
    doPacking(b,this->isAttacker_var);
    doPacking(b,this->attackKind_var);
}

void IDSVotingResponsePkt::parsimUnpack(cCommBuffer *b)
{
    cPacket::parsimUnpack(b);
    doUnpacking(b,this->destAddr_var);
    doUnpacking(b,this->srcAddr_var);
    doUnpacking(b,this->nodeID_var);
    doUnpacking(b,this->isAttacker_var);
    doUnpacking(b,this->attackKind_var);
}

LAddress::L3Type& IDSVotingResponsePkt::getDestAddr()
{
    return destAddr_var;
}

void IDSVotingResponsePkt::setDestAddr(const LAddress::L3Type& destAddr)
{
    this->destAddr_var = destAddr;
}

LAddress::L3Type& IDSVotingResponsePkt::getSrcAddr()
{
    return srcAddr_var;
}

void IDSVotingResponsePkt::setSrcAddr(const LAddress::L3Type& srcAddr)
{
    this->srcAddr_var = srcAddr;
}

int IDSVotingResponsePkt::getNodeID() const
{
    return nodeID_var;
}

void IDSVotingResponsePkt::setNodeID(int nodeID)
{
    this->nodeID_var = nodeID;
}

bool IDSVotingResponsePkt::getIsAttacker() const
{
    return isAttacker_var;
}

void IDSVotingResponsePkt::setIsAttacker(bool isAttacker)
{
    this->isAttacker_var = isAttacker;
}

Attack_t& IDSVotingResponsePkt::getAttackKind()
{
    return attackKind_var;
}

void IDSVotingResponsePkt::setAttackKind(const Attack_t& attackKind)
{
    this->attackKind_var = attackKind;
}

class IDSVotingResponsePktDescriptor : public cClassDescriptor
{
  public:
    IDSVotingResponsePktDescriptor();
    virtual ~IDSVotingResponsePktDescriptor();

    virtual bool doesSupport(cObject *obj) const;
    virtual const char *getProperty(const char *propertyname) const;
    virtual int getFieldCount(void *object) const;
    virtual const char *getFieldName(void *object, int field) const;
    virtual int findField(void *object, const char *fieldName) const;
    virtual unsigned int getFieldTypeFlags(void *object, int field) const;
    virtual const char *getFieldTypeString(void *object, int field) const;
    virtual const char *getFieldProperty(void *object, int field, const char *propertyname) const;
    virtual int getArraySize(void *object, int field) const;

    virtual std::string getFieldAsString(void *object, int field, int i) const;
    virtual bool setFieldAsString(void *object, int field, int i, const char *value) const;

    virtual const char *getFieldStructName(void *object, int field) const;
    virtual void *getFieldStructPointer(void *object, int field, int i) const;
};

Register_ClassDescriptor(IDSVotingResponsePktDescriptor);

IDSVotingResponsePktDescriptor::IDSVotingResponsePktDescriptor() : cClassDescriptor("IDSVotingResponsePkt", "cPacket")
{
}

IDSVotingResponsePktDescriptor::~IDSVotingResponsePktDescriptor()
{
}

bool IDSVotingResponsePktDescriptor::doesSupport(cObject *obj) const
{
    return dynamic_cast<IDSVotingResponsePkt *>(obj)!=NULL;
}

const char *IDSVotingResponsePktDescriptor::getProperty(const char *propertyname) const
{
    cClassDescriptor *basedesc = getBaseClassDescriptor();
    return basedesc ? basedesc->getProperty(propertyname) : NULL;
}

int IDSVotingResponsePktDescriptor::getFieldCount(void *object) const
{
    cClassDescriptor *basedesc = getBaseClassDescriptor();
    return basedesc ? 5+basedesc->getFieldCount(object) : 5;
}

unsigned int IDSVotingResponsePktDescriptor::getFieldTypeFlags(void *object, int field) const
{
    cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount(object))
            return basedesc->getFieldTypeFlags(object, field);
        field -= basedesc->getFieldCount(object);
    }
    static unsigned int fieldTypeFlags[] = {
        FD_ISCOMPOUND,
        FD_ISCOMPOUND,
        FD_ISEDITABLE,
        FD_ISEDITABLE,
        FD_ISCOMPOUND,
    };
    return (field>=0 && field<5) ? fieldTypeFlags[field] : 0;
}

const char *IDSVotingResponsePktDescriptor::getFieldName(void *object, int field) const
{
    cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount(object))
            return basedesc->getFieldName(object, field);
        field -= basedesc->getFieldCount(object);
    }
    static const char *fieldNames[] = {
        "destAddr",
        "srcAddr",
        "nodeID",
        "isAttacker",
        "attackKind",
    };
    return (field>=0 && field<5) ? fieldNames[field] : NULL;
}

int IDSVotingResponsePktDescriptor::findField(void *object, const char *fieldName) const
{
    cClassDescriptor *basedesc = getBaseClassDescriptor();
    int base = basedesc ? basedesc->getFieldCount(object) : 0;
    if (fieldName[0]=='d' && strcmp(fieldName, "destAddr")==0) return base+0;
    if (fieldName[0]=='s' && strcmp(fieldName, "srcAddr")==0) return base+1;
    if (fieldName[0]=='n' && strcmp(fieldName, "nodeID")==0) return base+2;
    if (fieldName[0]=='i' && strcmp(fieldName, "isAttacker")==0) return base+3;
    if (fieldName[0]=='a' && strcmp(fieldName, "attackKind")==0) return base+4;
    return basedesc ? basedesc->findField(object, fieldName) : -1;
}

const char *IDSVotingResponsePktDescriptor::getFieldTypeString(void *object, int field) const
{
    cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount(object))
            return basedesc->getFieldTypeString(object, field);
        field -= basedesc->getFieldCount(object);
    }
    static const char *fieldTypeStrings[] = {
        "LAddress::L3Type",
        "LAddress::L3Type",
        "int",
        "bool",
        "Attack_t",
    };
    return (field>=0 && field<5) ? fieldTypeStrings[field] : NULL;
}

const char *IDSVotingResponsePktDescriptor::getFieldProperty(void *object, int field, const char *propertyname) const
{
    cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount(object))
            return basedesc->getFieldProperty(object, field, propertyname);
        field -= basedesc->getFieldCount(object);
    }
    switch (field) {
        default: return NULL;
    }
}

int IDSVotingResponsePktDescriptor::getArraySize(void *object, int field) const
{
    cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount(object))
            return basedesc->getArraySize(object, field);
        field -= basedesc->getFieldCount(object);
    }
    IDSVotingResponsePkt *pp = (IDSVotingResponsePkt *)object; (void)pp;
    switch (field) {
        default: return 0;
    }
}

std::string IDSVotingResponsePktDescriptor::getFieldAsString(void *object, int field, int i) const
{
    cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount(object))
            return basedesc->getFieldAsString(object,field,i);
        field -= basedesc->getFieldCount(object);
    }
    IDSVotingResponsePkt *pp = (IDSVotingResponsePkt *)object; (void)pp;
    switch (field) {
        case 0: {std::stringstream out; out << pp->getDestAddr(); return out.str();}
        case 1: {std::stringstream out; out << pp->getSrcAddr(); return out.str();}
        case 2: return long2string(pp->getNodeID());
        case 3: return bool2string(pp->getIsAttacker());
        case 4: {std::stringstream out; out << pp->getAttackKind(); return out.str();}
        default: return "";
    }
}

bool IDSVotingResponsePktDescriptor::setFieldAsString(void *object, int field, int i, const char *value) const
{
    cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount(object))
            return basedesc->setFieldAsString(object,field,i,value);
        field -= basedesc->getFieldCount(object);
    }
    IDSVotingResponsePkt *pp = (IDSVotingResponsePkt *)object; (void)pp;
    switch (field) {
        case 2: pp->setNodeID(string2long(value)); return true;
        case 3: pp->setIsAttacker(string2bool(value)); return true;
        default: return false;
    }
}

const char *IDSVotingResponsePktDescriptor::getFieldStructName(void *object, int field) const
{
    cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount(object))
            return basedesc->getFieldStructName(object, field);
        field -= basedesc->getFieldCount(object);
    }
    static const char *fieldStructNames[] = {
        "LAddress::L3Type",
        "LAddress::L3Type",
        NULL,
        NULL,
        "Attack_t",
    };
    return (field>=0 && field<5) ? fieldStructNames[field] : NULL;
}

void *IDSVotingResponsePktDescriptor::getFieldStructPointer(void *object, int field, int i) const
{
    cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount(object))
            return basedesc->getFieldStructPointer(object, field, i);
        field -= basedesc->getFieldCount(object);
    }
    IDSVotingResponsePkt *pp = (IDSVotingResponsePkt *)object; (void)pp;
    switch (field) {
        case 0: return (void *)(&pp->getDestAddr()); break;
        case 1: return (void *)(&pp->getSrcAddr()); break;
        case 4: return (void *)(&pp->getAttackKind()); break;
        default: return NULL;
    }
}


