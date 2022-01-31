#ifndef SOCKET_HPP
#define SOCKET_HPP

#include "wideusb/communication/socket-queue.hpp"
#include "wideusb/communication/network-types.hpp"
#include "wideusb/communication/i-network-layer.hpp"
#include "wideusb/communication/utils/time-group-maker.hpp"
#include <functional>
#include <queue>
#include <memory>

class NetService;

class AddressFilter
{
public:
    /**
     * @brief Enable listening gived address. Accept if (incoming target & mask) == (addr & mask)
     * @param addr
     * @param mask
     */
    void listen_address(Address addr, Address mask);
    bool is_acceptable(const Address& addr) const;

private:
    struct Target
    {
        Address addr;
        Address mask;
    };
    std::vector<Target> m_targets;
};

struct SocketOptions
{
    SocketOptions(Address address) :
        address(address)
    { }
    Port port = 0;
    Address address;
    uint8_t ttl = 10;
    TimePlanningOptions retransmitting_options{1000ms, 100ms, 10, 5s};

    uint32_t input_queue_limit = 20;
    uint32_t output_queue_limit = 20;

    bool need_acknoledgement = true;
};

class ISocketSystemSide;

struct SocketState
{
    SocketState();
    void clear();

    enum class OutgoingState
    {
        clear = 0,
        waiting,
    };

    SegmentID outgoing_segment_id = 0;
    OutgoingState state = OutgoingState::clear;
};

class ISocketUserSide
{
public:
    struct IncomingMessage
    {
        Address sender;
        PBuffer data;
    };

    using OnIncomingDataCallback = std::function<void(ISocketUserSide& socket)>;
    using OnDataReceivedCallback = std::function<void(uint32_t id, bool success)>;

    virtual ~ISocketUserSide() = default;
    virtual SegmentID send(Address destination, PBuffer data) = 0;
    virtual std::optional<IncomingMessage> get_incoming() = 0;
    virtual SocketOptions& options() = 0;
    virtual AddressFilter& address_filter() = 0;
    virtual bool has_incoming() = 0;
    virtual void drop_currently_sending() = 0;
};

class ISocketSystemSide
{
public:
    struct OutgoingMessage
    {
        Address receiver;
        PBuffer data;
        uint32_t id = 0;
    };

    virtual ~ISocketSystemSide() = default;
    virtual OutgoingMessage get_outgoing() = 0;
    virtual bool has_outgoing() = 0;
    virtual void notify_outgoing_sending_done(SegmentID id, bool success) = 0;
    virtual void push_incoming(Address sender, PBuffer data) = 0;
    virtual const SocketOptions& get_options() = 0;
    virtual const AddressFilter& get_address_filter() = 0;
    virtual SocketState& state() = 0;
};


template<typename T>
class IQueue
{
public:
    using Ptr = std::shared_ptr<IQueue<T>>;

    virtual ~IQueue() = default;

    virtual void push(const T& data) = 0;
    virtual T front() = 0;
    virtual void pop() = 0;
    virtual size_t size() = 0;
    virtual size_t capacity() = 0;
};

class IQueueFactory
{
public:
    using Ptr = std::shared_ptr<IQueueFactory>;
    virtual ~IQueueFactory() = default;

    virtual IQueue<ISocketSystemSide::OutgoingMessage>::Ptr produce_outgoing_queue(size_t size) = 0;
    virtual IQueue<ISocketUserSide::IncomingMessage>::Ptr produce_incomming_queue(size_t size) = 0;
};

class Socket : public ISocketUserSide, public ISocketSystemSide
{
public:
    Socket(NetService& net_service,
           Address my_address,
           Port port,
           OnIncomingDataCallback incoming_cb = nullptr,
           OnDataReceivedCallback received_cb = nullptr);

    ~Socket();

    AddressFilter& address_filter() override;
    SocketOptions& options() override;

    // ISocketUserSide
    SegmentID send(Address destination, PBuffer data) override;
    std::optional<IncomingMessage> get_incoming() override;
    bool has_incoming() override;
    void drop_currently_sending() override;

protected:
    NetService& m_net_service;
    SocketOptions m_options;
    Port m_port;
    AddressFilter m_filter;

    SocketState m_state;

    uint32_t m_id_counter = 0;
    OnIncomingDataCallback m_incoming_cb;
    OnDataReceivedCallback m_received_cb;

private:
    // ISocketSystemSide
    OutgoingMessage get_outgoing() override;
    bool has_outgoing() override;
    void notify_outgoing_sending_done(SegmentID id, bool success) override;
    void push_incoming(Address sender, PBuffer data) override;

    const SocketOptions& get_options() override;
    const AddressFilter& get_address_filter() override;
    SocketState& state() override;

    IQueue<IncomingMessage>::Ptr m_incoming_queue;
    IQueue<OutgoingMessage>::Ptr m_outgoing_queue;
};

#endif // SOCKET_HPP
