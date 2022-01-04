//
// Generated file, do not edit! Created by nedtool 4.6 from src/node/communication/routing/sBDSDSN/SBDSDSN.msg.
//

// Disable warnings about unused variables, empty switch stmts, etc:
#ifdef _MSC_VER
#  pragma warning(disable:4101)
#  pragma warning(disable:4065)
#endif

#include <iostream>
#include <sstream>
#include "SBDSDSN_m.h"

USING_NAMESPACE


// Another default rule (prevents compiler from choosing base class' doPacking())
template<typename T>
void doPacking(cCommBuffer *, T& t) {
    throw cRuntimeError("Parsim error: no doPacking() function for type %s or its base class (check .msg and _m.cc/h files!)",opp_typename(typeid(t)));
}

template<typename T>
void doUnpacking(cCommBuffer *, T& t) {
    throw cRuntimeError("Parsim error: no doUnpacking() function for type %s or its base class (check .msg and _m.cc/h files!)",opp_typename(typeid(t)));
}




// Template rule for outputting std::vector<T> types
template<typename T, typename A>
inline std::ostream& operator<<(std::ostream& out, const std::vector<T,A>& vec)
{
    out.put('{');
    for(typename std::vector<T,A>::const_iterator it = vec.begin(); it != vec.end(); ++it)
    {
        if (it != vec.begin()) {
            out.put(','); out.put(' ');
        }
        out << *it;
    }
    out.put('}');
    
    char buf[32];
    sprintf(buf, " (size=%u)", (unsigned int)vec.size());
    out.write(buf, strlen(buf));
    return out;
}

// Template rule which fires if a struct or class doesn't have operator<<
template<typename T>
inline std::ostream& operator<<(std::ostream& out,const T&) {return out;}

EXECUTE_ON_STARTUP(
    cEnum *e = cEnum::find("SBDSDSNPacketDef");
    if (!e) enums.getInstance()->add(e = new cEnum("SBDSDSNPacketDef"));
    e->insert(SBD_SDSN_TRAFFIC_GENERAL, "SBD_SDSN_TRAFFIC_GENERAL");
    e->insert(SBD_DATA_PACKET_REMOTE_UPDATE, "SBD_DATA_PACKET_REMOTE_UPDATE");
    e->insert(SBD_BEACON_PACKET, "SBD_BEACON_PACKET");
    e->insert(SBD_JOIN_PACKET, "SBD_JOIN_PACKET");
    e->insert(SBD_TDMA_SCHEDULE_PACKET, "SBD_TDMA_SCHEDULE_PACKET");
    e->insert(SBD_RANGE_QUERY_PACKET, "SBD_RANGE_QUERY_PACKET");
    e->insert(SDSN_FLOW_REQ_CONTROLLER, "SDSN_FLOW_REQ_CONTROLLER");
);

Register_Class(SBDSDSNPacket);

SBDSDSNPacket::SBDSDSNPacket(const char *name, int kind) : ::RoutingPacket(name,kind)
{
    this->SBDSDSNPacketKind_var = 0;
    this->destSectorHeadID_var = 0;
    slot_arraysize = 0;
    this->slot_var = 0;
    segmentAddress_arraysize = 0;
    this->segmentAddress_var = 0;
    this->nextHop_var = 0;
    this->prevHop_var = 0;
    this->sectorSlotNumber_var = 0;
}

SBDSDSNPacket::SBDSDSNPacket(const SBDSDSNPacket& other) : ::RoutingPacket(other)
{
    slot_arraysize = 0;
    this->slot_var = 0;
    segmentAddress_arraysize = 0;
    this->segmentAddress_var = 0;
    copy(other);
}

SBDSDSNPacket::~SBDSDSNPacket()
{
    delete [] slot_var;
    delete [] segmentAddress_var;
}

SBDSDSNPacket& SBDSDSNPacket::operator=(const SBDSDSNPacket& other)
{
    if (this==&other) return *this;
    ::RoutingPacket::operator=(other);
    copy(other);
    return *this;
}

void SBDSDSNPacket::copy(const SBDSDSNPacket& other)
{
    this->SBDSDSNPacketKind_var = other.SBDSDSNPacketKind_var;
    this->destSectorHeadID_var = other.destSectorHeadID_var;
    delete [] this->slot_var;
    this->slot_var = (other.slot_arraysize==0) ? NULL : new int[other.slot_arraysize];
    slot_arraysize = other.slot_arraysize;
    for (unsigned int i=0; i<slot_arraysize; i++)
        this->slot_var[i] = other.slot_var[i];
    delete [] this->segmentAddress_var;
    this->segmentAddress_var = (other.segmentAddress_arraysize==0) ? NULL : new int[other.segmentAddress_arraysize];
    segmentAddress_arraysize = other.segmentAddress_arraysize;
    for (unsigned int i=0; i<segmentAddress_arraysize; i++)
        this->segmentAddress_var[i] = other.segmentAddress_var[i];
    this->nextHop_var = other.nextHop_var;
    this->prevHop_var = other.prevHop_var;
    this->sectorSlotNumber_var = other.sectorSlotNumber_var;
}

void SBDSDSNPacket::parsimPack(cCommBuffer *b)
{
    ::RoutingPacket::parsimPack(b);
    doPacking(b,this->SBDSDSNPacketKind_var);
    doPacking(b,this->destSectorHeadID_var);
    b->pack(slot_arraysize);
    doPacking(b,this->slot_var,slot_arraysize);
    b->pack(segmentAddress_arraysize);
    doPacking(b,this->segmentAddress_var,segmentAddress_arraysize);
    doPacking(b,this->nextHop_var);
    doPacking(b,this->prevHop_var);
    doPacking(b,this->sectorSlotNumber_var);
}

void SBDSDSNPacket::parsimUnpack(cCommBuffer *b)
{
    ::RoutingPacket::parsimUnpack(b);
    doUnpacking(b,this->SBDSDSNPacketKind_var);
    doUnpacking(b,this->destSectorHeadID_var);
    delete [] this->slot_var;
    b->unpack(slot_arraysize);
    if (slot_arraysize==0) {
        this->slot_var = 0;
    } else {
        this->slot_var = new int[slot_arraysize];
        doUnpacking(b,this->slot_var,slot_arraysize);
    }
    delete [] this->segmentAddress_var;
    b->unpack(segmentAddress_arraysize);
    if (segmentAddress_arraysize==0) {
        this->segmentAddress_var = 0;
    } else {
        this->segmentAddress_var = new int[segmentAddress_arraysize];
        doUnpacking(b,this->segmentAddress_var,segmentAddress_arraysize);
    }
    doUnpacking(b,this->nextHop_var);
    doUnpacking(b,this->prevHop_var);
    doUnpacking(b,this->sectorSlotNumber_var);
}

int SBDSDSNPacket::getSBDSDSNPacketKind() const
{
    return SBDSDSNPacketKind_var;
}

void SBDSDSNPacket::setSBDSDSNPacketKind(int SBDSDSNPacketKind)
{
    this->SBDSDSNPacketKind_var = SBDSDSNPacketKind;
}

int SBDSDSNPacket::getDestSectorHeadID() const
{
    return destSectorHeadID_var;
}

void SBDSDSNPacket::setDestSectorHeadID(int destSectorHeadID)
{
    this->destSectorHeadID_var = destSectorHeadID;
}

void SBDSDSNPacket::setSlotArraySize(unsigned int size)
{
    int *slot_var2 = (size==0) ? NULL : new int[size];
    unsigned int sz = slot_arraysize < size ? slot_arraysize : size;
    for (unsigned int i=0; i<sz; i++)
        slot_var2[i] = this->slot_var[i];
    for (unsigned int i=sz; i<size; i++)
        slot_var2[i] = 0;
    slot_arraysize = size;
    delete [] this->slot_var;
    this->slot_var = slot_var2;
}

unsigned int SBDSDSNPacket::getSlotArraySize() const
{
    return slot_arraysize;
}

int SBDSDSNPacket::getSlot(unsigned int k) const
{
    if (k>=slot_arraysize) throw cRuntimeError("Array of size %d indexed by %d", slot_arraysize, k);
    return slot_var[k];
}

void SBDSDSNPacket::setSlot(unsigned int k, int slot)
{
    if (k>=slot_arraysize) throw cRuntimeError("Array of size %d indexed by %d", slot_arraysize, k);
    this->slot_var[k] = slot;
}

void SBDSDSNPacket::setSegmentAddressArraySize(unsigned int size)
{
    int *segmentAddress_var2 = (size==0) ? NULL : new int[size];
    unsigned int sz = segmentAddress_arraysize < size ? segmentAddress_arraysize : size;
    for (unsigned int i=0; i<sz; i++)
        segmentAddress_var2[i] = this->segmentAddress_var[i];
    for (unsigned int i=sz; i<size; i++)
        segmentAddress_var2[i] = 0;
    segmentAddress_arraysize = size;
    delete [] this->segmentAddress_var;
    this->segmentAddress_var = segmentAddress_var2;
}

unsigned int SBDSDSNPacket::getSegmentAddressArraySize() const
{
    return segmentAddress_arraysize;
}

int SBDSDSNPacket::getSegmentAddress(unsigned int k) const
{
    if (k>=segmentAddress_arraysize) throw cRuntimeError("Array of size %d indexed by %d", segmentAddress_arraysize, k);
    return segmentAddress_var[k];
}

void SBDSDSNPacket::setSegmentAddress(unsigned int k, int segmentAddress)
{
    if (k>=segmentAddress_arraysize) throw cRuntimeError("Array of size %d indexed by %d", segmentAddress_arraysize, k);
    this->segmentAddress_var[k] = segmentAddress;
}

const char * SBDSDSNPacket::getNextHop() const
{
    return nextHop_var.c_str();
}

void SBDSDSNPacket::setNextHop(const char * nextHop)
{
    this->nextHop_var = nextHop;
}

const char * SBDSDSNPacket::getPrevHop() const
{
    return prevHop_var.c_str();
}

void SBDSDSNPacket::setPrevHop(const char * prevHop)
{
    this->prevHop_var = prevHop;
}

int SBDSDSNPacket::getSectorSlotNumber() const
{
    return sectorSlotNumber_var;
}

void SBDSDSNPacket::setSectorSlotNumber(int sectorSlotNumber)
{
    this->sectorSlotNumber_var = sectorSlotNumber;
}

class SBDSDSNPacketDescriptor : public cClassDescriptor
{
  public:
    SBDSDSNPacketDescriptor();
    virtual ~SBDSDSNPacketDescriptor();

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

Register_ClassDescriptor(SBDSDSNPacketDescriptor);

SBDSDSNPacketDescriptor::SBDSDSNPacketDescriptor() : cClassDescriptor("SBDSDSNPacket", "RoutingPacket")
{
}

SBDSDSNPacketDescriptor::~SBDSDSNPacketDescriptor()
{
}

bool SBDSDSNPacketDescriptor::doesSupport(cObject *obj) const
{
    return dynamic_cast<SBDSDSNPacket *>(obj)!=NULL;
}

const char *SBDSDSNPacketDescriptor::getProperty(const char *propertyname) const
{
    cClassDescriptor *basedesc = getBaseClassDescriptor();
    return basedesc ? basedesc->getProperty(propertyname) : NULL;
}

int SBDSDSNPacketDescriptor::getFieldCount(void *object) const
{
    cClassDescriptor *basedesc = getBaseClassDescriptor();
    return basedesc ? 7+basedesc->getFieldCount(object) : 7;
}

unsigned int SBDSDSNPacketDescriptor::getFieldTypeFlags(void *object, int field) const
{
    cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount(object))
            return basedesc->getFieldTypeFlags(object, field);
        field -= basedesc->getFieldCount(object);
    }
    static unsigned int fieldTypeFlags[] = {
        FD_ISEDITABLE,
        FD_ISEDITABLE,
        FD_ISARRAY | FD_ISEDITABLE,
        FD_ISARRAY | FD_ISEDITABLE,
        FD_ISEDITABLE,
        FD_ISEDITABLE,
        FD_ISEDITABLE,
    };
    return (field>=0 && field<7) ? fieldTypeFlags[field] : 0;
}

const char *SBDSDSNPacketDescriptor::getFieldName(void *object, int field) const
{
    cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount(object))
            return basedesc->getFieldName(object, field);
        field -= basedesc->getFieldCount(object);
    }
    static const char *fieldNames[] = {
        "SBDSDSNPacketKind",
        "destSectorHeadID",
        "slot",
        "segmentAddress",
        "nextHop",
        "prevHop",
        "sectorSlotNumber",
    };
    return (field>=0 && field<7) ? fieldNames[field] : NULL;
}

int SBDSDSNPacketDescriptor::findField(void *object, const char *fieldName) const
{
    cClassDescriptor *basedesc = getBaseClassDescriptor();
    int base = basedesc ? basedesc->getFieldCount(object) : 0;
    if (fieldName[0]=='S' && strcmp(fieldName, "SBDSDSNPacketKind")==0) return base+0;
    if (fieldName[0]=='d' && strcmp(fieldName, "destSectorHeadID")==0) return base+1;
    if (fieldName[0]=='s' && strcmp(fieldName, "slot")==0) return base+2;
    if (fieldName[0]=='s' && strcmp(fieldName, "segmentAddress")==0) return base+3;
    if (fieldName[0]=='n' && strcmp(fieldName, "nextHop")==0) return base+4;
    if (fieldName[0]=='p' && strcmp(fieldName, "prevHop")==0) return base+5;
    if (fieldName[0]=='s' && strcmp(fieldName, "sectorSlotNumber")==0) return base+6;
    return basedesc ? basedesc->findField(object, fieldName) : -1;
}

const char *SBDSDSNPacketDescriptor::getFieldTypeString(void *object, int field) const
{
    cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount(object))
            return basedesc->getFieldTypeString(object, field);
        field -= basedesc->getFieldCount(object);
    }
    static const char *fieldTypeStrings[] = {
        "int",
        "int",
        "int",
        "int",
        "string",
        "string",
        "int",
    };
    return (field>=0 && field<7) ? fieldTypeStrings[field] : NULL;
}

const char *SBDSDSNPacketDescriptor::getFieldProperty(void *object, int field, const char *propertyname) const
{
    cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount(object))
            return basedesc->getFieldProperty(object, field, propertyname);
        field -= basedesc->getFieldCount(object);
    }
    switch (field) {
        case 0:
            if (!strcmp(propertyname,"enum")) return "SBDSDSNPacketDef";
            return NULL;
        default: return NULL;
    }
}

int SBDSDSNPacketDescriptor::getArraySize(void *object, int field) const
{
    cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount(object))
            return basedesc->getArraySize(object, field);
        field -= basedesc->getFieldCount(object);
    }
    SBDSDSNPacket *pp = (SBDSDSNPacket *)object; (void)pp;
    switch (field) {
        case 2: return pp->getSlotArraySize();
        case 3: return pp->getSegmentAddressArraySize();
        default: return 0;
    }
}

std::string SBDSDSNPacketDescriptor::getFieldAsString(void *object, int field, int i) const
{
    cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount(object))
            return basedesc->getFieldAsString(object,field,i);
        field -= basedesc->getFieldCount(object);
    }
    SBDSDSNPacket *pp = (SBDSDSNPacket *)object; (void)pp;
    switch (field) {
        case 0: return long2string(pp->getSBDSDSNPacketKind());
        case 1: return long2string(pp->getDestSectorHeadID());
        case 2: return long2string(pp->getSlot(i));
        case 3: return long2string(pp->getSegmentAddress(i));
        case 4: return oppstring2string(pp->getNextHop());
        case 5: return oppstring2string(pp->getPrevHop());
        case 6: return long2string(pp->getSectorSlotNumber());
        default: return "";
    }
}

bool SBDSDSNPacketDescriptor::setFieldAsString(void *object, int field, int i, const char *value) const
{
    cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount(object))
            return basedesc->setFieldAsString(object,field,i,value);
        field -= basedesc->getFieldCount(object);
    }
    SBDSDSNPacket *pp = (SBDSDSNPacket *)object; (void)pp;
    switch (field) {
        case 0: pp->setSBDSDSNPacketKind(string2long(value)); return true;
        case 1: pp->setDestSectorHeadID(string2long(value)); return true;
        case 2: pp->setSlot(i,string2long(value)); return true;
        case 3: pp->setSegmentAddress(i,string2long(value)); return true;
        case 4: pp->setNextHop((value)); return true;
        case 5: pp->setPrevHop((value)); return true;
        case 6: pp->setSectorSlotNumber(string2long(value)); return true;
        default: return false;
    }
}

const char *SBDSDSNPacketDescriptor::getFieldStructName(void *object, int field) const
{
    cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount(object))
            return basedesc->getFieldStructName(object, field);
        field -= basedesc->getFieldCount(object);
    }
    switch (field) {
        default: return NULL;
    };
}

void *SBDSDSNPacketDescriptor::getFieldStructPointer(void *object, int field, int i) const
{
    cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount(object))
            return basedesc->getFieldStructPointer(object, field, i);
        field -= basedesc->getFieldCount(object);
    }
    SBDSDSNPacket *pp = (SBDSDSNPacket *)object; (void)pp;
    switch (field) {
        default: return NULL;
    }
}


