#include <iostream>//std::cerr;
#include <string>//std::string
#include "server.h"

std::string PACKET_POST_URL;

int main(int argc, char* argv[])
{
  try
  {
    if (argc != 3)
    {
      std::cerr << "Usage: async_tcp_echo_server <port> <packet_post_url>\n";
      return 1;
    }

    boost::asio::io_service io_service;

    PACKET_POST_URL = argv[2];

    Server s(io_service, std::atoi(argv[1]));

    io_service.run();
  }
  catch (std::exception& e)
  {
    std::cerr << "Exception: " << e.what() << "\n";
  }

  return 0;
}
