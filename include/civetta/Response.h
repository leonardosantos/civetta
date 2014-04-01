#ifndef _CIVETTA_RESPONSE_H
#define _CIVETTA_RESPONSE_H

#include <map>
#include <sstream>
#include <iostream>

#include <civetta/Common.h>

/**
 * A response to a request
 */
namespace Civetta {

class CIVETTA_EXPORT Response : public std::ostringstream {
public:
  enum codes {
    OK = 200,
    NOT_FOUND = 404,
    FORBIDDEN = 403,
    SERVER_ERROR = 500,
    BAD_REQUEST = 401
  };
  Response();

  /**
   * Test if the given header is present
   *
   * @param string the header key
   *
   * @return bool true if the header is set
   */
  bool hasHeader(std::string key);

  /**
   * Sets the header
   *
   * @param key the header key
   *
   * @param value the header value
   */
  void setHeader(std::string key, std::string value);

  /**
   * Get the data of the response, this will contain headers and
   * body
   *
   * @return string the response data
   */
  std::string getData();

  /**
   * Gets the response body
   *
   * @return string the response body
   */
  std::string getBody();

  /**
   * Sets the cookie, note that you can only define one cookie by request
   * for now
   *
   * @param string the key of the cookie
   * @param string value the cookie value
   */
  void setCookie(std::string key, std::string value);

  /**
   * Sets the response code
   */
  void setCode(int code);

protected:
  int code;
  std::map<std::string, std::string> headers;
};
}

#endif
