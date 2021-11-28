#include "wideusb/back/dac-back.hpp"
#include "wideusb/communication/modules/dac.hpp"

DACModuleBackend::DACModuleBackend(NetService& net_service, Address module_address) :
    m_sock_setup(net_service, module_address, dac::setup::port, [this](ISocketUserSide&) { sock_setup_listener(); }),
    m_sock_data(net_service, module_address, dac::data::port, [this](ISocketUserSide&) { sock_data_listener(); })
{
}

void DACModuleBackend::sock_setup_listener()
{
    while (m_sock_setup.has_data())
    {
        Socket::IncomingMessage incoming = *m_sock_setup.get();
        auto req_init_continious = try_interpret_buffer_magic<dac::setup::InitContinious>(incoming.data);
        if (req_init_continious)
        {
            uint16_t error_code = init_continious(
                        req_init_continious->chunk_size,
                        req_init_continious->timings.prescaler,
                        req_init_continious->timings.period);

            dac::setup::InitResponse response;
            response.error_code = error_code;

            m_sock_setup.send(incoming.sender, Buffer::serialize(response));
            continue;
        }

        auto req_init_sample = try_interpret_buffer_magic<dac::setup::InitSample>(incoming.data);
        if (req_init_sample)
        {
            uint16_t error_code = init_sample(
                        req_init_sample->buffer_size,
                        req_init_sample->timings.prescaler,
                        req_init_sample->timings.period,
                        req_init_sample->autorepeat != 0);

            dac::setup::InitResponse response;
            response.error_code = error_code;

            m_sock_setup.send(incoming.sender, Buffer::serialize(response));
            continue;
        }

        auto req_run_stop = try_interpret_buffer_magic<dac::setup::RunRequest>(incoming.data);
        if (req_run_stop)
        {
            if (req_run_stop->run_stop == dac::setup::RunRequest::run)
            {
                m_status_reports_receiver = req_run_stop->status_reports_receiver;
                run();
            } else if (req_run_stop->run_stop == dac::setup::RunRequest::stop)
            {
                stop();
            } else if (req_run_stop->run_stop == dac::setup::RunRequest::stop_immediately)
            {
                stop_immediately();
            }
            continue;
        }

    }
}

void DACModuleBackend::sock_data_listener()
{
    while (m_sock_data.has_data())
    {
        Socket::IncomingMessage incoming = *m_sock_data.get();
        receive_data(incoming.data);
    }
}

void DACModuleBackend::send_notification_buffer_is_short(uint16_t size)
{
    dac::data::BufferIsShortNotification notification;
    notification.buffer_size = size;
    m_sock_data.send(m_status_reports_receiver, Buffer::serialize(notification));
}
