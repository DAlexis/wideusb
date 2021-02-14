#ifndef NETWORKING_HPP
#define NETWORKING_HPP

#include "communication/network-base.hpp"
#include "communication/i-physical-layer.hpp"
#include "communication/i-channel-layer.hpp"
#include "communication/i-network-layer.hpp"
#include "communication/i-transport-layer.hpp"
#include "communication/utils/time-group-maker.hpp"
#include "buffer.hpp"

#include <vector>
#include <map>
#include <set>
#include <functional>
#include <optional>

class NetSevice;

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
    SocketOptions(const NetworkOptions& net_opts) :
        network_options(net_opts)
    { }
    uint32_t port = 0;
    NetworkOptions network_options;
    TimePlanningOptions retransmitting_options{1000, 100, 10, 5000};

    bool need_acknoledgement = true;
    /*uint32_t timeout_ms = 60000;
    uint32_t repeat_interval_ms = 1000;
    uint32_t repeats_limit = 10;*/
};

class ISocketSystemSide;

struct SocketState
{
    SocketState();
    void clear();

    enum class OutgoingState
    {
        clear = 0,
        repeating_untill_ack
    };

    uint32_t segment_id = 0;
    OutgoingState state = OutgoingState::clear;
    /*bool state_clear = true;
    uint32_t last_send_time = 0;
    uint32_t repeats_count = 0;*/
};

class ISocketUserSide
{
public:
    using OnDataReceivedCallback = std::function<void(uint32_t id, bool success)>;
    virtual ~ISocketUserSide() = default;
    virtual uint32_t send(PBuffer data) = 0;
    virtual PBuffer get() = 0;
    virtual SocketOptions& options() = 0;
    virtual AddressFilter& address_filter() = 0;
    virtual bool has_data() = 0;
    virtual void drop_currently_sending() = 0;
};

class ISocketSystemSide
{
public:
    struct SocketData
    {
        PBuffer data;
        uint32_t id = 0;
    };

    virtual ~ISocketSystemSide() = default;
    virtual PBuffer front() = 0;
    virtual bool has_outgoing_data() = 0;
    virtual void pop(bool success) = 0;
    virtual void push(PBuffer data) = 0;
    virtual const SocketOptions& get_options() = 0;
    virtual const AddressFilter& get_address_filter() = 0;
    virtual SocketState& state() = 0;
};

class Socket : public ISocketUserSide, public ISocketSystemSide
{
public:
    Socket(NetSevice& net_service,
           Address my_address,
           Address destination_address,
           uint32_t port,
           OnDataReceivedCallback callback = nullptr);
    ~Socket();

    // ISocketUserSide
    uint32_t send(PBuffer data) override;
    PBuffer get() override;
    SocketOptions& options() override;
    AddressFilter& address_filter() override;
    bool has_data() override;
    void drop_currently_sending() override;

private:
    // ISocketSystemSide
    PBuffer front() override;
    bool has_outgoing_data() override;
    void pop(bool success) override;
    void push(PBuffer data) override;
    const SocketOptions& get_options() override;
    const AddressFilter& get_address_filter() override;
    SocketState& state() override;

    NetSevice& m_net_service;
    SocketOptions m_options;
    uint32_t m_port;
    AddressFilter m_filter;
    std::list<PBuffer> m_incoming;
    std::list<SocketData> m_outgoing;

    SocketState m_state;

    uint32_t m_id_counter = 0;
    OnDataReceivedCallback m_callback;
};

class NetSevice
{
public:
    using RandomGenerator = std::function<uint32_t(void)>;
    NetSevice(
            std::shared_ptr<IPhysicalLayer> physical,
            std::shared_ptr<IChannelLayer> channel,
            std::shared_ptr<INetworkLayer> network,
            std::shared_ptr<ITransportLayer> transport,
            std::shared_ptr<IPhysicalLayer> default_transit_physical = nullptr,
            RandomGenerator rand_gen = nullptr);

    void serve_sockets(uint32_t time_ms);

    void add_socket(Socket& socket);
    void remove_socket(Socket& socket);

    uint32_t generate_segment_id();

private:

    void send_all_sockets(uint32_t time_ms);
    void receive_all_sockets();
    void serve_time_planner(uint32_t time_ms);

    void send_data(PBuffer data, Address src, Address dst, uint32_t port, uint32_t ttl, bool need_ack, uint32_t seg_id);
    uint32_t send_ack(Address src, Address dst, uint32_t port, uint32_t ttl, uint32_t ack_id, uint32_t seg_id);

    bool is_already_received(uint32_t segment_id);

    std::vector<ISocketSystemSide*> receivers_of_addr(Address addr);

    std::shared_ptr<IPhysicalLayer> m_physical;
    std::shared_ptr<IChannelLayer> m_channel;
    std::shared_ptr<INetworkLayer> m_network;
    std::shared_ptr<ITransportLayer> m_transport;

    std::shared_ptr<IPhysicalLayer> m_default_transit_physical;

    std::vector<ISocketSystemSide*> m_subscribers;
    RandomGenerator m_rand_gen;

    const size_t m_already_received_capacity = 100;
    std::list<uint32_t> m_already_received;

    TimePlanner<ISocketSystemSide*> m_time_planner; // TODO
};

#endif // NETWORKING_HPP
