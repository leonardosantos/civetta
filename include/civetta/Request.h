#ifndef _CIVETTA_REQUEST_H
#define _CIVETTA_REQUEST_H

#include <iostream>
#include <sstream>
#include <vector>
#include <regex>
#include <map>
#include <civetweb.h>
#include <civetta/Common.h>

namespace Civetta {
/**
 * Request is a wrapper for the clients requests
 */
class CIVETTA_EXPORT Request {
public:
  Request(struct mg_connection *connection, std::smatch matches);
  std::smatch getMatches();
  
protected:
  std::smatch matches;
  const struct mg_connection *connection;
  const struct mg_request_info *request_info;
};
}

#endif
