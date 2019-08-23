#include "message.hpp"  
#include "timestamp.hpp"

using namespace pebo;

BaseMessage::BaseMessage(messageType type_in, timestamp_t createdTime_in) :
myType(type_in),
myCreatedTime(createdTime_in)
{
    if (myCreatedTime == 0)
    {
        myCreatedTime = TimeStamp::now();
    }
}

PeerUpdateMessage::PeerUpdateMessage(service_t service_in, endpoint_t endpoint_in, timestamp_t lastSeen_in, bool isRemoved_in, timestamp_t createdTime_in) :
BaseMessage(messageType::PeerUpdate, createdTime_in),
myService(service_in),
myEndpoint(endpoint_in),
myLastSeen(lastSeen_in),
myIsRemoved(isRemoved_in)
{
}

PeerUpdateMessage::PeerUpdateMessage(PeerInfo const & peerInfo_in, timestamp_t createdTime_in) :
BaseMessage(messageType::PeerUpdate, createdTime_in),
myService(peerInfo_in.service),
myEndpoint(peerInfo_in.endpoint),
myLastSeen(peerInfo_in.lastSeen),
myIsRemoved(peerInfo_in.isRemoved)
{
}

void PeerUpdateMessage::visit(MessageVisitorBase & visitor_in) const
{
    visitor_in.peerUpdate(*this);
}

std::string PeerUpdateMessage::toString() const
{
    return "PEER " + getService() + " " + getEndpoint() + " " + std::to_string(getLastSeen());
}


QueryMessage::QueryMessage(service_t service_in, timestamp_t createdTime_in) :
BaseMessage(messageType::Query, createdTime_in),
myService(service_in)
{
}

void QueryMessage::visit(MessageVisitorBase & visitor_in) const
{
    visitor_in.query(*this);
}

std::string QueryMessage::toString() const
{
    return "QUERY " + getService();
}