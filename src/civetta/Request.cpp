#include <string.h>
#include <string>
#include <iostream>
#include <civetweb.h>
#include <civetta/Request.h>

using namespace std;

namespace Civetta {

Request::Request(struct mg_connection *connection_, smatch matches_)
    : connection(connection_), matches(matches_),
      request_info(mg_get_request_info(connection_)) {}

smatch Request::getMatches() { return matches; }
};
