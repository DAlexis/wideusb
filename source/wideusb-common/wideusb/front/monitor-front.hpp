#ifndef MONITORFRONT_HPP
#define MONITORFRONT_HPP

#include "wideusb/front/base-front.hpp"
#include "wideusb/communication/networking.hpp"
#include "wideusb/utils/utilities.hpp"
#include "wideusb/modules/monitor-types.hpp"

class MonitorFront : public ModuleFrontBase
{
public:
    using StatusReceivedCallbackEntry = CallbackEntry<const std::string&>;
    using ConnectedToStdoutCallbackEntry = CallbackEntry<bool>;
    using OnStdoutDataReceivedCallbackEntry = CallbackEntry<const std::string&>;

    MonitorFront(NetService::ptr host_connection_service, OnModuleCreatedCallbackEntry on_created, Address my_address, Address device_address);
    void get_status_async(StatusReceivedCallbackEntry receiver);
    void connect_to_stdout(ConnectedToStdoutCallbackEntry on_connected, OnStdoutDataReceivedCallbackEntry on_data_received);

    std::string get_stdout();

private:
    void request_status();
    void socket_listener_status();
    void socket_listener_stdout();

    Socket m_status_socket;
    Socket m_stdout_socket;
    StatusReceivedCallbackEntry m_status_receiver;
    ConnectedToStdoutCallbackEntry m_on_connected_to_stdout;
    OnStdoutDataReceivedCallbackEntry m_on_data_received;
};



#endif // MONITORFRONT_HPP
