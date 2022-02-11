#ifndef SOCKET_HPP
#define SOCKET_HPP

#include "wideusb/communication/socket-queue.hpp"
#include "wideusb/communication/network-types.hpp"
#include "wideusb/communication/i-network-layer.hpp"
#include <functional>
#include <queue>
#include <memory>
#include <chrono>

using namespace std::literals::chrono_literals;

class NetService;
class DecodedSegment;

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

struct TimePlanningOptions
{

    TimePlanningOptions(std::chrono::milliseconds interval = 1000ms,
                        std::chrono::milliseconds duration = 100ms,
                        uint32_t cycles_count = 0,
                        std::chrono::milliseconds timeout = 0ms) :
        duration(duration), interval(interval), cycles_count(cycles_count), timeout(timeout)
    { }

    TimePlanningOptions& operator=(const TimePlanningOptions&) = default;

    std::chrono::milliseconds duration; ///< Task may be peeked during duration
    std::chrono::milliseconds interval;
    uint32_t cycles_count;
    std::chrono::milliseconds timeout;
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
    virtual SegmentID send(Address destination, PBuffer data, std::chrono::steady_clock::time_point now = std::chrono::steady_clock::now()) = 0;
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
        uint32_t id = 0;
        bool is_only_ack = false;
        uint16_t sended_times = 0;
        uint16_t cycles_count = 0;
        std::chrono::steady_clock::time_point next_send;
        SegmentBuffer packet;
    };

    virtual ~ISocketSystemSide() = default;

    virtual std::optional<std::chrono::steady_clock::time_point> next_send_time() = 0;
    virtual std::optional<SegmentBuffer> pick_outgoing_packet(std::chrono::steady_clock::time_point now) = 0;
    virtual void receive_segment(std::chrono::steady_clock::time_point now, Address sender, const DecodedSegment& segment, bool duplicate) = 0;

    virtual const SocketOptions& get_options() = 0;
    virtual const AddressFilter& get_address_filter() = 0;
};


template<typename T>
class IQueue
{
public:
    using Ptr = std::shared_ptr<IQueue<T>>;

    virtual ~IQueue() = default;

    virtual void push(const T& data) = 0;
    virtual void push_front(const T& data) = 0;
    virtual T& front() = 0;
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
    Socket(std::shared_ptr<NetService> net_service,
           Address my_address,
           Port port,
           OnIncomingDataCallback incoming_cb = nullptr,
           OnDataReceivedCallback received_cb = nullptr);

    ~Socket();

    AddressFilter& address_filter() override;
    SocketOptions& options() override;

    // ISocketUserSide
    SegmentID send(Address destination, PBuffer data, std::chrono::steady_clock::time_point now = std::chrono::steady_clock::now()) override;
    std::optional<IncomingMessage> get_incoming() override;
    bool has_incoming() override;
    void drop_currently_sending() override;

protected:
    std::shared_ptr<NetService> m_net_service;
    SocketOptions m_options;
    Port m_port;
    AddressFilter m_filter;

    uint32_t m_id_counter = 0;
    OnIncomingDataCallback m_incoming_cb;
    OnDataReceivedCallback m_received_cb;

private:
    // ISocketSystemSide
    std::optional<std::chrono::steady_clock::time_point> next_send_time() override;
    std::optional<SegmentBuffer> pick_outgoing_packet(std::chrono::steady_clock::time_point now) override;
    void receive_segment(std::chrono::steady_clock::time_point now, Address sender, const DecodedSegment& segment, bool duplicate) override;

    const SocketOptions& get_options() override;
    const AddressFilter& get_address_filter() override;

    IQueue<IncomingMessage>::Ptr m_incoming_queue;
    IQueue<OutgoingMessage>::Ptr m_outgoing_queue;
};

#endif // SOCKET_HPP
