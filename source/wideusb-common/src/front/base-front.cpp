#include "wideusb/front/base-front.hpp"
#include "wideusb/communication/modules/core.hpp"

ModuleFrontBase::ModuleFrontBase(NetService& host_connection_service, uint32_t module_id, Address device_address, Address host_address, OnModuleCreatedCallback on_created) :
    m_host_connection_service(host_connection_service),
    m_device_address(device_address), m_host_address(host_address),
    m_module_id(module_id), m_on_created(on_created)
{
    create_module();
    //m_host_connection_service.connect_module(*this);
}

ModuleFrontBase::~ModuleFrontBase()
{
    //m_host_connection_service.remove_module(*this);
}

void ModuleFrontBase::create_module()
{
    m_create_module_socket.reset(
        new Socket(
            m_host_connection_service, m_host_address, ports::core::create_module,
            [this](ISocketUserSide&){ create_module_socket_listener(); }
        )
    );

    //m_create_module_socket->address_filter().listen_address(0x00000000, 0x00000000); // Any

    core::create_module::Request request;
    request.module_id = m_module_id;
    PBuffer body = Buffer::create(sizeof(request), &request);
    m_create_module_socket->send(m_device_address, body);
    //std::cout << "Sending create module request..." << std::endl;
}

void ModuleFrontBase::create_module_socket_listener()
{
    Socket::IncomingMessage incoming = *m_create_module_socket->get_incoming();
    //std::cout << "Create module response received from " << incoming.sender << std::endl;

    bool success = false;

    m_create_module_socket.reset();

    core::create_module::Response response;
    if (incoming.data->size() == sizeof(response))
    {
        BufferAccessor(incoming.data) >> response;
        if (response.module_id == m_module_id && response.success)
            success = true;
    }

    //std::cout << "module creation result: " << success << std::endl;

    if (m_on_created)
        m_on_created(success);
}
