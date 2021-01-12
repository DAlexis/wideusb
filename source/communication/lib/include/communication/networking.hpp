#ifndef NETWORKING_HPP
#define NETWORKING_HPP

#include "communication/network-base.hpp"
#include "communication/i-physical-layer.hpp"
#include "communication/i-channel-layer.hpp"
#include "communication/i-network-layer.hpp"
#include "communication/i-transport-layer.hpp"
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
    uint32_t port = 0;
    Address my_address = 0;
    Address destination_address = 0;
    bool need_acknoledgement = true;
    uint32_t timeout_ms = 60000;
    uint32_t repeat_interval_ms = 1000;
    uint32_t repeats_limit = 10;
    uint8_t ttl = 10;
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
};

class ISocketSystemSide
{
public:
    virtual ~ISocketSystemSide() = default;
    virtual PBuffer front() = 0;
    virtual void pop(bool success) = 0;
    virtual void push(PBuffer data) = 0;
    virtual const SocketOptions& get_options() = 0;
    virtual const AddressFilter& get_address_filter() = 0;
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

    // ISocketSystemSide
    virtual PBuffer front() override;
    void pop(bool success) override;
    void push(PBuffer data) override;
    const SocketOptions& get_options() override;
    const AddressFilter& get_address_filter() override;

private:
    struct OutgoingItem
    {
        PBuffer data;
        uint32_t id = 0;
    };

    NetSevice& m_net_service;
    SocketOptions m_options;
    uint32_t m_port;
    AddressFilter m_filter;
    std::list<PBuffer> m_incoming;
    std::list<OutgoingItem> m_outgoing;

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
            RandomGenerator rand_gen = nullptr);

    void serve_sockets(uint32_t time_ms);

    void add_socket(Socket& socket);
    void remove_socket(Socket& socket);



private:
    struct Subscriber
    {
        Subscriber();
        void clear();

        enum class State
        {
            clear = 0,
            some_tries_sent
        };

        ISocketSystemSide* socket = nullptr;
        uint32_t segment_id = 0;
        State state = State::clear;
        bool state_clear = true;
        uint32_t last_send_time = 0;
        uint32_t repeats_count = 0;
    };

    void send_all_sockets(uint32_t time_ms);
    void receive_all_sockets();

    void send_data(PBuffer data, Address src, Address dst, uint32_t port, uint32_t ttl, bool need_ack, uint32_t seg_id);
    uint32_t send_ack(Address src, Address dst, uint32_t port, uint32_t ttl, uint32_t ack_id, uint32_t seg_id);

    bool is_already_received(uint32_t segment_id);

    //void send_ack(Address orig_sender, Address orig_reciver, uint32_t orig_msg_id, uint32_t );

    std::vector<Subscriber*> receivers_of_addr(Address addr);
    //void filter_by_port(std::vector<Socket*>& sockets, uint32_t port);

    std::shared_ptr<IPhysicalLayer> m_physical;
    std::shared_ptr<IChannelLayer> m_channel;
    std::shared_ptr<INetworkLayer> m_network;
    std::shared_ptr<ITransportLayer> m_transport;

    std::vector<Subscriber> m_subscribers;
    RandomGenerator m_rand_gen;

    const size_t m_already_received_capacity = 100;
    std::list<uint32_t> m_already_received;
};

#endif // NETWORKING_HPP
