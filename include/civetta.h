/* Copyright (c) 2013-2014 the Civetta developers
 * Copyright (c) 2004-2013 Sergey Lyubka
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 */

#ifndef _CIVETTA_H
#define _CIVETTA_H

#ifdef _MSC_VER
#pragma once
#pragma warning(disable : 4251)  // Disable VC warning about dll linkage required
                                 // (for private members?)
#pragma warning(disable : 4275)  // Disable non dll-interface base class warning
#endif

#if defined(_MSC_VER) && defined(CIVETTA_USE_DLL)
#ifdef CIVETTA_BUILD_DLL
#define CIVETTA_EXPORT __declspec(dllexport)
#else
#define CIVETTA_EXPORT __declspec(dllimport)
#endif
#else
#define CIVETTA_EXPORT
#endif

#include <map>
#include <string>
#include <functional>
#include <iostream>
#include <sstream>
#include <vector>
#include <regex>

#include "civetweb.h"

namespace Civetta {

class Server;

/**
  Request is a wrapper for the clients requests
*/
class CIVETTA_EXPORT Request {
 public:
  /**
    Request constructor.
    @param connection the request connection
  */
  Request(struct mg_connection *connection);
  virtual ~Request();

  /**
    Information about the request.
  */
  const struct mg_request_info *request_info;

  /**
    Accessor for the request uri matches.
    @return the uri route pattern matches
  */
  std::smatch getMatches();

  /**
     Gets a cookie.
     @param cookieName - cookie name to get the value from
     @param cookieValue - cookie value is returned using this reference
     @puts the cookie value string that matches the cookie name in the _cookieValue string.
     @returns the size of the cookie value string read.
  */
  int getCookie(const std::string &cookieName, std::string &cookieValue);

  /**
     Gets a header.
     @param headerName - header name to get the value from
     @returns a char array whcih contains the header value as
     string
  */
  const char *getHeader(const std::string &headerName);
  
  /**
     Gets a param.
     Returns a query paramter contained in the supplied buffer.
     The occurence value is a zero-based index of a particular key name.
     This should not be confused with the index over all of the keys.
     @param name the key to search for
     @param the destination string
     @param occurrence the occurrence of the selected name in the query (0 based).
     @return true of key was found
  */
  bool getParam(const char *name, std::string &dst, size_t occurrence = 0);

  /**
     Gets a param as Array.
     Returns array of strings containing the values passed in the varios occurrences of the param.
     @param name the key to search for
     @param the destination string
     @param occurrence the occurrence of the selected name in the query (0 based).
     @return true of key was found
  */
  std::vector<std::string> getParamArray(const char *name);
  
  /**
     Gets a query string param.
     Returns a query paramter contained in the supplied buffer.
     The occurence value is a zero-based index of a particular key name.
     This should not be confused with the index over all of the keys.
     @param name the key to search for
     @param the destination string
     @param occurrence the occurrence of the selected name in the query (0 based).
     @return true of key was found
  */
  bool getQueryParam(const char *name, std::string &dst, size_t occurrence = 0);

  /**
     Gets a query string param as Array.
     Returns array of strings containing the values passed in the varios occurrences of the param.
     @param name the key to search for
     @param the destination string
     @param occurrence the occurrence of the selected name in the query (0 based).
     @return true of key was found
  */
  std::vector<std::string> getQueryParamArray(const char *name);

  const char* getPostData();

  std::vector<std::string> getUploads(std::string destination_path);

 protected:
  char *postData;
  std::map<std::string, std::vector<std::string> > values;
  std::map<std::string, std::vector<std::string> > query;
  struct mg_connection *connection;
  std::smatch matches;
  std::vector<std::string> upload_filepaths;

  friend class Server;
};

class CIVETTA_EXPORT Response : public std::ostringstream {
 public:
  /**
    Some of the HTTP response codes
  */
  enum codes { OK = 200, NOT_FOUND = 404, FORBIDDEN = 403, SERVER_ERROR = 500, BAD_REQUEST = 400 };
  Response();

  /**
    Test if the given header is present
    @param string the header key
    @return bool true if the header is set
  */
  bool hasHeader(std::string key);

  /**
     Sets the header
     @param key the header key
     @param value the header value
   */
  void setHeader(std::string key, std::string value);

  /**
     Get the data of the response, this will contain headers and body.
     @return string the response data
   */
  std::string getData();

  /**
     Gets the response body.
     @return string the response body
   */
  std::string getBody();

  /**
     Sets the cookie, note that you can only define one cookie by request for now.
     @param string the key of the cookie
     @param string value the cookie value
   */
  void setCookie(std::string key, std::string value);

  /**
     Sets the response code
   */
  void setCode(int code);

 protected:
  int code;
  std::map<std::string, std::string> headers;
};

typedef std::function<void(Request &request, Response &response)> Callback;

/**
  Basic class for embedded web server.  This has a URL mapping built-in.
 */
class CIVETTA_EXPORT Server {
 public:
  /**
     Constructor.

     This automatically starts the sever.
     It is good practice to call getContext() after this in case there
     were errors starting the server.

     @param options - the web server options.
     @param callbacks - optional web server callback methods.
   */
  Server(const char **options, const struct mg_callbacks *callbacks = 0);

  /**
     Destructor
   */
  virtual ~Server();

  /**
    Stops server and frees resources.
  */
  void close();

  /**
     Accessor for the server context.
     @return the context or 0 if not running.
  */
  const struct mg_context *getContext() const { return context; }

  /**
     Registers a route to the server
     @param string the method
     @param string the url path
     @param Callback the request handler for this route
   */
  Callback route(std::string httpMethod, std::string url, Callback callback);
  
  /**
     Sets the routes prefix.
   */
  void setPrefix(std::string prefix);
  void setUploadDestination(std::string upload_destination);
  std::string getUploadDestination() const;

 protected:
  struct mg_context *context;
  std::map<std::string, Callback> routes;
  std::string prefix;
  std::string upload_destination;

 private:
  /**
     Handles the incomming request.
     @param conn - the connection information
     @param cbdata - pointer to the CivetHandler instance.
     @returns 0 if implemented, false otherwise
   */
  int requestHandler(struct mg_connection *conn, void *cbdata);
  static int globalHandler(struct mg_connection *conn, void *cbdata);

  /**
     Handles closing a request (internal handler)
     @param conn - the connection information
   */
  static void closeHandler(struct mg_connection *conn);

  static void uploadHandler(struct mg_connection *conn, const char *file_name);

  /**
     Stores the user provided close handler
   */
  void (*userCloseHandler)(struct mg_connection *conn);
  void (*userUploadHandler)(struct mg_connection *conn, const char *filename);
};

class CIVETTA_EXPORT Util {
 public:
  /**
     Url decoder.
     @param src buffer to be decoded
     @param src_len length of buffer to be decoded
     @param dst destination string
     @param is_form_url_encoded true if form url encoded form-url-encoded data differs from URI encoding in a way that
     it uses '+' as character for space, see RFC 1866 section 8.2.1 http://ftp.ics.uci.edu/pub/ietf/html/rfc1866.txt
   */
  static void urlDecode(const char *src, size_t src_len, std::string &dst, bool is_form_url_encoded = true);

  /**
     Url decoder.
     @param src string to be decoded
     @param dst destination string
     @param is_form_url_encoded true if form url encoded form-url-encoded data differs from URI encoding in a way that
     it uses '+' as character for space, see RFC 1866 section 8.2.1 http://ftp.ics.uci.edu/pub/ietf/html/rfc1866.txt
   */
  static void urlDecode(const std::string &src, std::string &dst, bool is_form_url_encoded = true);

  /**
     Url decoder.
     @param src buffer to be decoded (0 terminated)
     @param dst destination string
     @param is_form_url_encoded true if form url encoded form-url-encoded data differs from URI encoding in a way that
     it uses '+' as character for space, see RFC 1866 section 8.2.1 http://ftp.ics.uci.edu/pub/ietf/html/rfc1866.txt
   */
  static void urlDecode(const char *src, std::string &dst, bool is_form_url_encoded = true);

  /**
     Url encoder.
     @param src buffer to be encoded
     @param src_len length of buffer to be decoded
     @param dst destination string
     @param append true if string should not be cleared before encoding.
   */
  static void urlEncode(const char *src, size_t src_len, std::string &dst, bool append = false);

  /**
     Url encoder.
     @param src buffer to be encoded (0 terminated)
     @param dst destination string
     @param append true if string should not be cleared before encoding.
   */
  static void urlEncode(const char *src, std::string &dst, bool append = false);

  /**
     Url encoder.
     @param src buffer to be encoded
     @param dst destination string
     @param append true if string should not be cleared before
     encoding.
   */
  static void urlEncode(const std::string &src, std::string &dst, bool append = false);
};
}  // namespace Civetta
#endif  // _CIVETTA_H
