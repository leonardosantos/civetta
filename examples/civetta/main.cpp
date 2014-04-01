#include <iostream>

#include "civetta.h"

int main(int argc, char *argv[]) {

  const char *options[] = { "document_root", ".", "listening_ports", "8081",
                            0 };

  Civetta::Server server(options);
  server.route("GET", "/",
               [](Civetta::Request &req,
                  Civetta::Response &res) { res << "Hello World!"; });

  std::cout << "Press <ENTER> to exit";
  std::cin.ignore();
  return 0;
}
