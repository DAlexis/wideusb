#ifndef PHYSICALLAYERTCPSERVER_HPP
#define PHYSICALLAYERTCPSERVER_HPP

#include "wideusb-common/buffer.hpp"
#include <boost/asio.hpp>
#include <memory>
#include <set>

using boost::asio::ip::tcp;

class TCPHub
{
public:
    TCPHub();

private:

};

class HubServer;

class HubSession : public std::enable_shared_from_this<HubSession>
{
public:
    HubSession(tcp::socket socket, HubServer& server);
    void start();
    void send(PBuffer buf);

    void stop();

private:
    constexpr static size_t incoming_buffer_size = 10;
    void do_read();

    tcp::socket m_socket;
    HubServer& m_server;
    PBuffer m_incoming_data;
};

class HubServer
{
public:
    HubServer(boost::asio::io_service& io_service, short port);

    void stop();

    void on_session_removed(std::shared_ptr<HubSession> session);
    void on_session_received_data(std::shared_ptr<HubSession> session, PBuffer data);

private:
    void do_accept();

    tcp::acceptor m_acceptor;
    tcp::socket m_socket;

    std::set<std::shared_ptr<HubSession>> m_sessions;
};
/*
int main(int argc, char* argv[])
{
  try
  {
    if (argc != 2)
    {
      std::cerr << "Usage: async_tcp_echo_server <port>\n";
      return 1;
    }

    boost::asio::io_service io_service;

    server s(io_service, std::atoi(argv[1]));

    io_service.run();
  }
  catch (std::exception& e)
  {
    std::cerr << "Exception: " << e.what() << "\n";
  }

  return 0;
}
*/
#endif // PHYSICALLAYERTCPSERVER_HPP
