#include "wideusb-pc/asio-utils.hpp"
#include "wideusb/communication/networking.hpp"

#include <iostream>

std::shared_ptr<IOServiceRunner> IOServiceRunner::create()
{
    return std::shared_ptr<IOServiceRunner>(new IOServiceRunner());
}

IOServiceRunner::IOServiceRunner() :
    m_work(std::make_shared<boost::asio::io_service::work>(m_io_service)),
    m_service_thread([this]() {
        boost::asio::io_service::work work(m_io_service);
        m_io_service.run();
        std::cout << "IO service thread done." << std::endl;
    })
{
}

IOServiceRunner::~IOServiceRunner()
{
    stop();
}

boost::asio::io_service& IOServiceRunner::io_service()
{
    return m_io_service;
}

void IOServiceRunner::stop()
{
    m_work.reset();
    m_io_service.stop();
    m_service_thread.join();
}

void IOServiceRunner::join()
{
    m_service_thread.join();
}

