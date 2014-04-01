/* Copyright (c) 2013-2014 the Civetta developers
 * Copyright (c) 2013 No Face Press, LLC
 *
 * License http://opensource.org/licenses/mit-license.php MIT License
 */

#ifndef _CIVETTA_SERVER_H_
#define _CIVETTA_SERVER_H_

#include <map>
#include <string>
#include <functional>
#include <civetweb.h>
#include <civetta/Common.h>
#include <civetta/Request.h>
#include <civetta/Response.h>

namespace Civetta {

typedef std::function<void(Request &request, Response &response)> Callback;

/**
 * Server
 *
 * Basic class for embedded web server.  This has a URL mapping built-in.
 */
class CIVETTA_EXPORT Server {
public:
  /**
   * Constructor
   *
   * This automatically starts the sever.
   * It is good practice to call getContext() after this in case there
   * were errors starting the server.
   *
   * @param options - the web server options.
   * @param callbacks - optional web server callback methods.
   */
  Server(const char **options, const struct mg_callbacks *callbacks = 0);

  /**
   * Destructor
   */
  virtual ~Server();

  /**
   * close()
   *
   * Stops server and frees resources.
   */
  void close();

  /**
   * getContext()
   *
   * @return the context or 0 if not running.
   */
  const struct mg_context *getContext() const { return context; }

  /**
   * getCookie(struct mg_connection *conn, const std::string &cookieName,
   * std::string &cookieValue)
   * @param conn - the connection information
   * @param cookieName - cookie name to get the value from
   * @param cookieValue - cookie value is returned using thiis reference
   * @puts the cookie value string that matches the cookie name in the
   * _cookieValue string.
   * @returns the size of the cookie value string read.
  */
  static int getCookie(struct mg_connection *conn,
                       const std::string &cookieName, std::string &cookieValue);

  /**
   * getHeader(struct mg_connection *conn, const std::string &headerName)
   * @param conn - the connection information
   * @param headerName - header name to get the value from
   * @returns a char array whcih contains the header value as string
  */
  static const char *getHeader(struct mg_connection *conn,
                               const std::string &headerName);

  /**
   * getParam(struct mg_connection *conn, const char *, std::string &, size_t)
   *
   * Returns a query paramter contained in the supplied buffer.  The
   * occurance value is a zero-based index of a particular key name.  This
   * should nto be confused with the index over all of the keys.
   *
   * @param data the query string
   * @param name the key to search for
   * @param the destination string
   * @param occurrence the occurrence of the selected name in the query (0
   *based).
   * @return true of key was found
   */
  static bool getParam(struct mg_connection *conn, const char *name,
                       std::string &dst, size_t occurrence = 0);

  /**
   * getParam(const std::string &, const char *, std::string &, size_t)
   *
   * Returns a query paramter contained in the supplied buffer.  The
   * occurance value is a zero-based index of a particular key name.  This
   * should nto be confused with the index over all of the keys.
   *
   * @param data the query string
   * @param name the key to search for
   * @param the destination string
   * @param occurrence the occurrence of the selected name in the query (0
   *based).
   * @return true of key was found
   */
  static bool getParam(const std::string &data, const char *name,
                       std::string &dst, size_t occurrence = 0) {
    return getParam(data.c_str(), data.length(), name, dst, occurrence);
  }

  /**
   * getParam(const char *, size_t, const char *, std::string &, size_t)
   *
   * Returns a query paramter contained in the supplied buffer.  The
   * occurance value is a zero-based index of a particular key name.  This
   * should nto be confused with the index over all of the keys.
   *
   * @param data the query string
   * @param length length of the query string
   * @param name the key to search for
   * @param the destination string
   * @param occurrence the occurrence of the selected name in the query (0
   *based).
   * @return true of key was found
   */
  static bool getParam(const char *data, size_t data_len, const char *name,
                       std::string &dst, size_t occurrence = 0);

    /**
     * Registers a route to the controller
     *
     * @param string the method
     * @param string the url path
     * @param Callback the request handler for this route
     */
    Callback route(std::string httpMethod, std::string url, Callback callback);

protected:
  struct mg_context *context;
  char *postData;
  std::map<std::string, Callback> routes;

private:
  /**
   * requestHandler(struct mg_connection *, void *cbdata)
   *
   * Handles the incomming request.
   *
   * @param conn - the connection information
   * @param cbdata - pointer to the CivetHandler instance.
   * @returns 0 if implemented, false otherwise
   */
  int requestHandler(struct mg_connection *conn, void *cbdata);
  static int globalHandler(struct mg_connection *conn, void * cbdata);

  /**
   * closeHandler(struct mg_connection *)
   *
   * Handles closing a request (internal handler)
   *
   * @param conn - the connection information
   */
  static void closeHandler(struct mg_connection *conn);

  /**
   * Stores the user provided close handler
   */
  void (*userCloseHandler)(struct mg_connection *conn);
};
}

#endif // _CIVETTA_SERVER_H_
