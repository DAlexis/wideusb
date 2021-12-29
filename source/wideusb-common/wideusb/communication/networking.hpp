#ifndef NETWORKING_HPP
#define NETWORKING_HPP

#include "wideusb/communication/network-types.hpp"
#include "wideusb/communication/i-physical-layer.hpp"
#include "wideusb/communication/i-channel-layer.hpp"
#include "wideusb/communication/i-network-layer.hpp"
#include "wideusb/communication/i-transport-layer.hpp"
#include "wideusb/communication/i-package-inspector.hpp"
#include "wideusb/communication/utils/time-group-maker.hpp"
#include "wideusb/buffer.hpp"

#include <vector>
#include <map>
#include <set>
#include <functional>
#include <optional>
#include <chrono>

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
    TimePlanningOptions retransmitting_options{std::chrono::milliseconds(1000), std::chrono::milliseconds(100), 10, std::chrono::milliseconds(5000)};

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
        repeating_untill_ack,
        repeating_untill_expired_no_ack
    };

    SegmentID segment_id = 0;
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
    virtual std::optional<IncomingMessage> get() = 0;
    virtual SocketOptions& options() = 0;
    virtual AddressFilter& address_filter() = 0;
    virtual bool has_data() = 0;
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
    virtual OutgoingMessage front() = 0;
    virtual bool has_outgoing_data() = 0;
    virtual void pop(bool success) = 0;
    virtual void push(Address sender, PBuffer data) = 0;
    virtual const SocketOptions& get_options() = 0;
    virtual const AddressFilter& get_address_filter() = 0;
    virtual SocketState& state() = 0;
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

    // ISocketUserSide
    SegmentID send(Address destination, PBuffer data) override;
    std::optional<IncomingMessage> get() override;
    SocketOptions& options() override;
    AddressFilter& address_filter() override;
    bool has_data() override;
    void drop_currently_sending() override;

private:
    // ISocketSystemSide
    OutgoingMessage front() override;
    bool has_outgoing_data() override;
    void pop(bool success) override;
    void push(Address sender, PBuffer data) override;
    const SocketOptions& get_options() override;
    const AddressFilter& get_address_filter() override;
    SocketState& state() override;

    NetService& m_net_service;
    SocketOptions m_options;
    Port m_port;
    AddressFilter m_filter;
    std::list<IncomingMessage> m_incoming;
    std::list<OutgoingMessage> m_outgoing;

    SocketState m_state;

    uint32_t m_id_counter = 0;
    OnIncomingDataCallback m_incoming_cb;
    OnDataReceivedCallback m_received_cb;
};

class NetService
{
public:
    using RandomGenerator = std::function<uint32_t(void)>;
    using OnAnySocketSendCallback = std::function<void(void)>;

    NetService(
            std::shared_ptr<IPhysicalLayer> physical,
            std::shared_ptr<IChannelLayer> channel,
            std::shared_ptr<INetworkLayer> network,
            std::shared_ptr<ITransportLayer> transport,
            std::shared_ptr<IPhysicalLayer> default_transit_physical = nullptr,
            OnAnySocketSendCallback on_any_socket_send = nullptr,
            std::shared_ptr<IPackageInspector> package_inspector = nullptr,
            RandomGenerator rand_gen = nullptr);

    void serve_sockets(std::chrono::steady_clock::time_point time_ms);

    void add_socket(Socket& socket);
    void remove_socket(Socket& socket);

    SegmentID generate_segment_id();

    void on_socket_send();

private:

    void serve_sockets_output(std::chrono::steady_clock::time_point time_ms);
    void serve_sockets_input();
    void serve_time_planner(std::chrono::steady_clock::time_point time_ms);

    void send_ack(Address src, Address dst, Port port, uint32_t ttl, uint32_t ack_id, SegmentID seg_id);

    bool is_already_received(SegmentID segment_id);

    std::vector<ISocketSystemSide*> receivers_of_addr(Address addr);

    std::shared_ptr<IPhysicalLayer> m_physical;
    std::shared_ptr<IChannelLayer> m_channel;
    std::shared_ptr<INetworkLayer> m_network;
    std::shared_ptr<ITransportLayer> m_transport;
    std::shared_ptr<IPackageInspector> m_package_inspector;

    std::shared_ptr<IPhysicalLayer> m_default_transit_physical;

    std::vector<ISocketSystemSide*> m_sockets;
    RandomGenerator m_rand_gen;
    OnAnySocketSendCallback m_on_any_socket_send_callback;

    const size_t m_already_received_capacity = 100;
    std::list<SegmentID> m_already_received;

    TimePlanner<ISocketSystemSide*> m_time_planner; // TODO
};

#endif // NETWORKING_HPP
