#include <iostream>

#include "civetta.h"

int main(int argc, char *argv[]) {

  const char *options[] = {"document_root", ".", "listening_ports", "8081", 0};

  Civetta::Server server(options);
  server.route("POST", "/", [](Civetta::Request &req, Civetta::Response &res) {
    std::string param;
    if(req.getParam("field", param))
      res << param << std::endl;
    if(req.getParam("file", param))
      res << param << std::endl;
  });

  server.route("GET", "/", [](Civetta::Request &req, Civetta::Response &res) {
    res << "<html><body>Upload example."
           "<form method=\"POST\" action=\"/\" "
           "  enctype=\"multipart/form-data\">"
           "<input type=\"text\" name=\"field\" /> <br/>"
           "<input type=\"file\" name=\"file\" /> <br/>"
           "<input type=\"submit\" value=\"Upload\" />"
           "</form></body></html>";
  });

  std::cout << "Press <ENTER> to exit";
  std::cin.ignore();
  return 0;
}
