#include "host-modules/dac.hpp"
#include "communication/modules/dac.hpp"
#include "wideusb.hpp"

using namespace WideUSBHost::modules;

DAC::DAC(Device& host_connection_service, OnModuleCreatedCallback on_created, Address custom_host_address, Address custom_device_address) :
    ModuleBase(
        host_connection_service, dac::id,
        custom_device_address != 0 ? custom_device_address : host_connection_service.device_address(),
        custom_host_address != 0 ? custom_host_address : host_connection_service.host_address(),
        on_created
    ),
    m_sock_setup(
        m_host_connection_service.net_service(),
        custom_host_address != 0 ? custom_host_address : m_host_connection_service.host_address(),
        dac::setup::port,
        [this](ISocketUserSide&) { sock_setup_listener(); }
    ),
    m_sock_data(
        m_host_connection_service.net_service(),
        custom_host_address != 0 ? custom_host_address : m_host_connection_service.host_address(),
        dac::data::port,
        [this](ISocketUserSide&) { sock_data_lestener(); }
    )
{
}

void DAC::init(bool repeat, OnInitDoneCallback on_init_done)
{
    dac::setup::InitRequest request;
    request.mode = repeat ? dac::setup::InitRequest::repeat : dac::setup::InitRequest::single;
    request.period = 1000;
    request.buffer_size = 1000;

    m_on_init_done = on_init_done;
    m_sock_setup.send(m_device_address, Buffer::serialize(request));
}


void DAC::stop()
{

}

void DAC::sock_setup_listener()
{
    ISocketUserSide::IncomingMessage incoming = *m_sock_setup.get();
    auto response = try_interpret_buffer_no_magic<dac::setup::InitResponse>(incoming.data);

    if (!response.has_value())
        return;

    if (m_on_init_done)
    {
        InitResult res;
        res.success = response->success;
        res.artual_buffer_size= response->actual_buffer_size;

        m_on_init_done(res);
    }
}

void DAC::sock_data_lestener()
{
}
