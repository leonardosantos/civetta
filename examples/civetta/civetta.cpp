#include <civetta\Server.h>
#include <iostream>

int main(int argc, char *argv[]) {

  const char *options[] = { "document_root", ".", "listening_ports", "8081",
                            0 };

  Civetta::Server server(options);
  server.route("GET", "/civetta",
               [](Civetta::Request &req,
                  Civetta::Response &res) { res << "Hello World!"; });

  std::cout << "Press <ENTER> to exit" << std::endl;
  std::cin.ignore();
  return 0;
}
