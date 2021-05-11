#ifndef HOST_MODULE_DAC_GPP
#define HOST_MODULE_DAC_GPP

#include "communication/networking.hpp"
#include "host-module.hpp"


namespace WideUSBHost
{

class Device;

namespace modules
{

class DAC : public ModuleBase
{
public:
    struct InitResult
    {
        bool success = false;
        size_t artual_buffer_size = 0;
    };
    using OnInitDoneCallback = std::function<void(InitResult)>;
    DAC(Device& host_connection_service, OnModuleCreatedCallback on_created, Address custom_host_address = 0, Address custom_device_address = 0);

    void init(bool repeat, OnInitDoneCallback on_init_done);

    void stop();

private:
    void sock_setup_listener();
    void sock_data_lestener();

    PBuffer m_data;

    Socket m_sock_setup;
    Socket m_sock_data;

    OnInitDoneCallback m_on_init_done;
};

}
}

#endif // HOST_MODULE_DAC_GPP
