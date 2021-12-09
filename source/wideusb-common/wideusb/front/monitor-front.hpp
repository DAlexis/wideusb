#ifndef MONITORFRONT_HPP
#define MONITORFRONT_HPP

#include "wideusb/front/base-front.hpp"
#include "wideusb/communication/networking.hpp"

class MonitorFront : public ModuleFrontBase
{
public:
    using StatusReceivedCallback = std::function<void(const std::string&)>;
    using OnConnectedToStdout = std::function<void(bool)>;
    using OnStdoutDataReceived = std::function<void(std::string)>;

    MonitorFront(NetService& host_connection_service, OnModuleCreatedCallback on_created, Address my_address, Address device_address);
    void get_status_async(StatusReceivedCallback callback);
    void connect_to_stdout(OnConnectedToStdout on_connected, OnStdoutDataReceived on_data_received);

    std::string get_stdout();

private:
    void request_status();
    void socket_listener_status();
    void socket_listener_stdout();

    Socket m_status_socket;
    Socket m_stdout_socket;
    StatusReceivedCallback m_on_status_updated;
    OnConnectedToStdout m_on_connected_to_stdout;
    OnStdoutDataReceived m_on_data_received;
};



#endif // MONITORFRONT_HPP
