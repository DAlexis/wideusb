#ifndef DAC_BACK_HPP
#define DAC_BACK_HPP

#include "wideusb/communication/networking.hpp"
#include "wideusb/back/base-back.hpp"

class DACModuleBackend : public IModule
{
public:
    DACModuleBackend(NetService::ptr net_service, Address module_address);

private:
    void sock_setup_listener();
    void sock_data_listener();

protected:
    virtual uint16_t init_continious(uint16_t dma_chunk_size, uint32_t prescaler, uint32_t period) = 0;
    virtual uint16_t init_sample(uint16_t buffer_size, uint32_t prescaler, uint32_t period, bool repeat) = 0;
    virtual void run() = 0;
    virtual void stop() = 0;
    virtual void stop_immediately() = 0;
    virtual void receive_data(PBuffer data) = 0;

    void send_notification_buffer_is_short(uint16_t size);

    Socket m_sock_setup;
    Socket m_sock_data;

    Address m_status_reports_receiver = 0;
};

#endif // DAC_BACK_HPP
