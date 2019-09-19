#pragma once
#include "../include/peerboot.hpp"

namespace pebo
{
    /**
     * Message type.
     */
    enum messageType : uint8_t
    {
        invalid = 0,
        PeerUpdate = 1,
        Query = 2
    };

    class MessageVisitorBase;  // forward decl

    /**
     * Base class for PeerBoot message
     */
    class BaseMessage
    {
    public:
        BaseMessage(messageType type_in, timestamp_t createdTime_in);
        virtual ~BaseMessage() = default;
        messageType getType() const { return myType; }
        virtual void visit(MessageVisitorBase & visitor_in) const = 0;
        virtual std::string toString() const = 0;

        static const std::string Prefix;

    private:
        messageType myType;
        timestamp_t myCreatedTime;
    };

    /**
     * Peer update info message
     */
    class PeerUpdateMessage: public BaseMessage
    {
    public:
        // createdTime_in: special value 0 means current time
        PeerUpdateMessage(service_t service_in, endpoint_t endpoint_in, timestamp_t lastSeen_in, bool isRemoved_in, timestamp_t createdTime_in);
        PeerUpdateMessage(PeerInfo const & peerInfo_in, timestamp_t createdTime_in);
        service_t getService() const { return myService; }
        endpoint_t getEndpoint() const { return myEndpoint; }
        timestamp_t getLastSeen() const { return myLastSeen; }
        bool getIsRemoved() const { return myIsRemoved; }
        void visit(MessageVisitorBase & visitor_in) const;
        std::string toString() const;
        
    private:
        service_t myService;
        endpoint_t myEndpoint;
        timestamp_t myLastSeen;
        bool myIsRemoved;
    };

    /**
     * Query service message
     */
    class QueryMessage: public BaseMessage
    {
    public:
        QueryMessage(service_t service_in, timestamp_t createdTime_in);
        service_t getService() const { return myService; }
        void visit(MessageVisitorBase & visitor_in) const;
        std::string toString() const;

    private:
        service_t myService;
    };

    class MessageVisitorBase
    {
    public:
	    virtual void peerUpdate(PeerUpdateMessage const & msg_in) = 0;
	    virtual void query(QueryMessage const & msg_in) = 0;
	    virtual ~MessageVisitorBase() = default;
    };
}
