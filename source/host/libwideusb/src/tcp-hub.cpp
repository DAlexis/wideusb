#include "wideusb/tcp-hub.hpp"
#include <iostream>

HubSession::HubSession(tcp::socket socket, HubServer& server) :
    m_socket(std::move(socket)),
    m_server(server)
{
}

void HubSession::start()
{
    do_read();
}

void HubSession::stop()
{
    m_socket.cancel();
}

void HubSession::send(PBuffer buf)
{
    auto self = shared_from_this();
    // buf is captured by lambda, so lifetime is prolongated until lambda is called
    boost::asio::async_write(m_socket, boost::asio::buffer(buf->data(), buf->size()),
        [this, buf, self](boost::system::error_code ec, std::size_t /*length*/)
        {
            if (ec)
            {
                std::cout << "Error during sending data to socket" << std::endl;
                m_server.on_session_removed(self);
                return;
                //do_read();
            }
        });
}

void HubSession::do_read()
{
    m_incoming_data = Buffer::create(incoming_buffer_size);
    auto self = shared_from_this();
    // this object will not be destroyed untill callback is done because lambda captures shared_ptr to this
    m_socket.async_read_some(boost::asio::buffer(m_incoming_data->data(), m_incoming_data->size()),
        [this, self](boost::system::error_code ec, std::size_t length)
        {
            if (ec == boost::asio::error::eof)
            {
                // Connection closed
                m_server.on_session_removed(self);
                return;
            }
            if (ec)
            {
                std::cout << "Error during receiving data from socket" << std::endl;
                m_server.on_session_removed(self);
                return;
            }
            m_server.on_session_received_data(self, m_incoming_data);
            do_read();
        });
}

HubServer::HubServer(boost::asio::io_service& io_service, short port)
    : m_acceptor(io_service, tcp::endpoint(tcp::v4(), port)), m_socket(io_service)
{
    do_accept();
}

void HubServer::stop()
{
    for (auto session: m_sessions)
    {
        session->stop();
    }
}

void HubServer::on_session_removed(std::shared_ptr<HubSession> session)
{
    m_sessions.erase(session);
}

void HubServer::on_session_received_data(std::shared_ptr<HubSession> session, PBuffer data)
{
    for (auto p_session : m_sessions)
    {
        if (p_session == session)
            continue;
        // TODO parse package and decrement ttl
        p_session->send(data);
    }
}

void HubServer::do_accept()
{
    m_acceptor.async_accept(m_socket,
        [this](boost::system::error_code ec)
        {
            if (!ec)
            {
                auto new_session = std::make_shared<HubSession>(std::move(m_socket), *this);
                new_session->start();
                m_sessions.insert(new_session);
            }
            do_accept();
        });
}
