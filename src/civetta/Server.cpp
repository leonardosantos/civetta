#include <assert.h>
#include <civetta/Util.h>
#include <civetta/Server.h>
#include <civetta/Request.h>

using namespace std;

namespace Civetta {

int Server::globalHandler(struct mg_connection *conn, void * cbdata){
  return ((Server *)cbdata)->requestHandler(conn, cbdata);
}

Server::Server(const char **options, const struct mg_callbacks *_callbacks)
    : context(0), postData(0) {
  struct mg_callbacks callbacks;
  memset(&callbacks, 0, sizeof(callbacks));
  if (_callbacks) {
    callbacks = *_callbacks;
    userCloseHandler = _callbacks->connection_close;
  } else {
    userCloseHandler = NULL;
  }
  callbacks.connection_close = closeHandler;

  context = mg_start(&callbacks, this, options);
  mg_set_request_handler(context, "", &Server::globalHandler, this);
}

Server::~Server() { close(); }

void Server::closeHandler(struct mg_connection *conn) {
  struct mg_request_info *request_info = mg_get_request_info(conn);
  assert(request_info != NULL);
  Server *me = (Server *)(request_info->user_data);
  assert(me != NULL);

  if (me->userCloseHandler)
    me->userCloseHandler(conn);
  if (me->postData) {
    free(me->postData);
    me->postData = 0;
  }
}

void Server::close() {
  if (context) {
    mg_stop(context);
    context = 0;
  }
}

int Server::getCookie(struct mg_connection *conn, const std::string &cookieName,
                      std::string &cookieValue) {
  // Maximum cookie length as per microsoft is 4096.
  // http://msdn.microsoft.com/en-us/library/ms178194.aspx
  char _cookieValue[4096];
  const char *cookie = mg_get_header(conn, "Cookie");
  int lRead = mg_get_cookie(cookie, cookieName.c_str(), _cookieValue,
                            sizeof(_cookieValue));
  cookieValue.clear();
  cookieValue.append(_cookieValue);
  return lRead;
}

const char *Server::getHeader(struct mg_connection *conn,
                              const std::string &headerName) {
  return mg_get_header(conn, headerName.c_str());
}

bool Server::getParam(struct mg_connection *conn, const char *name,
                      std::string &dst, size_t occurrence) {
  const char *formParams = NULL;
  struct mg_request_info *ri = mg_get_request_info(conn);
  assert(ri != NULL);
  Server *me = (Server *)(ri->user_data);
  assert(me != NULL);

  if (me->postData != NULL) {
    formParams = me->postData;
  } else {
    const char *con_len_str = mg_get_header(conn, "Content-Length");
    if (con_len_str) {
      unsigned long con_len = atoi(con_len_str);
      if (con_len > 0) {
        me->postData = (char *)malloc(con_len);
        if (me->postData != NULL) {
          /* malloc may fail for huge requests */
          mg_read(conn, me->postData, con_len);
          formParams = me->postData;
        }
      }
    }
  }
  if (formParams == NULL) {
    // get requests do store html <form> field values in the http query_string
    formParams = ri->query_string;
  }

  if (formParams != NULL) {
    return getParam(formParams, strlen(formParams), name, dst, occurrence);
  }

  return false;
}

bool Server::getParam(const char *data, size_t data_len, const char *name,
                      std::string &dst, size_t occurrence) {
  const char *p, *e, *s;
  size_t name_len;

  dst.clear();
  if (data == NULL || name == NULL || data_len == 0) {
    return false;
  }
  name_len = strlen(name);
  e = data + data_len;

  // data is "var1=val1&var2=val2...". Find variable first
  for (p = data; p + name_len < e; p++) {
    if ((p == data || p[-1] == '&') && p[name_len] == '=' &&
        !mg_strncasecmp(name, p, name_len) && 0 == occurrence--) {

      // Point p to variable value
      p += name_len + 1;

      // Point s to the end of the value
      s = (const char *)memchr(p, '&', (size_t)(e - p));
      if (s == NULL) {
        s = e;
      }
      assert(s >= p);

      // Decode variable into destination buffer
      Util::urlDecode(p, (size_t)(s - p), dst, true);
      return true;
    }
  }
  return false;
}

Callback Server::route(string httpMethod, string url, Callback callback) {
  string key = httpMethod + ":" + url + "/?";
  routes[key] = callback;
  return callback;
}

int Server::requestHandler(struct mg_connection *conn, void *cbdata) {
  struct mg_request_info *request_info = mg_get_request_info(conn);
  assert(request_info != NULL);
  Server *me = (Server *)(request_info->user_data);
  assert(me != NULL);

  smatch matches;
  string key, data;
  for (auto it = routes.begin(); it != routes.end(); it++) {
    key =
        string(request_info->request_method) + ":" + string(request_info->uri);
    if (regex_match(key, matches, regex(it->first))) {
      Request request(conn, matches);
      Response *response = new Response();
      it->second(request, *response);
      data = response->getData();
      mg_write(conn, data.c_str(), data.size());
      return 1;
    }
  }

  return 0;
}
}
