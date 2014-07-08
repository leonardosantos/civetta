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

#include <assert.h>
#include <map>
#include "civetta.h"

using namespace std;

namespace Civetta {

void Util::urlDecode(const string &src, string &dst, bool is_form_url_encoded) {
  urlDecode(src.c_str(), src.length(), dst, is_form_url_encoded);
}

void Util::urlDecode(const char *src, string &dst, bool is_form_url_encoded) {
  urlDecode(src, strlen(src), dst, is_form_url_encoded);
}

void Util::urlDecode(const char *src, size_t src_len, string &dst, bool is_form_url_encoded) {
  int i, j, a, b;
#define HEXTOI(x) (isdigit(x) ? x - '0' : x - 'W')
  dst.clear();
  for (i = j = 0; i < (int)src_len; i++, j++)
    if (src[i] == '%' && i < (int)src_len - 2 && isxdigit(*(const unsigned char *)(src + i + 1)) &&
        isxdigit(*(const unsigned char *)(src + i + 2))) {
      a = tolower(*(const unsigned char *)(src + i + 1));
      b = tolower(*(const unsigned char *)(src + i + 2));
      dst.push_back((char)((HEXTOI(a) << 4) | HEXTOI(b)));
      i += 2;
    } else if (is_form_url_encoded && src[i] == '+') {
      dst.push_back(' ');
    } else {
      dst.push_back(src[i]);
    }
}

void Util::urlEncode(const string &src, string &dst, bool append) {
  urlEncode(src.c_str(), src.length(), dst, append);
}

void Util::urlEncode(const char *src, string &dst, bool append) {
  urlEncode(src, strlen(src), dst, append);
}

void Util::urlEncode(const char *src, size_t src_len, string &dst, bool append) {
  static const char *dont_escape = "._-$,;~()";
  static const char *hex = "0123456789abcdef";

  if (!append)
    dst.clear();

  for (; src_len > 0; src++, src_len--)
    if (isalnum(*(const unsigned char *)src) || strchr(dont_escape, *(const unsigned char *)src) != NULL) {
      dst.push_back(*src);
    } else {
      dst.push_back('%');
      dst.push_back(hex[(*(const unsigned char *)src) >> 4]);
      dst.push_back(hex[(*(const unsigned char *)src) & 0xf]);
    }
}

int Server::globalHandler(struct mg_connection *conn, void *cbdata) {
  return ((Server *)cbdata)->requestHandler(conn, cbdata);
}

Server::Server(const char **options, const struct mg_callbacks *_callbacks) : context(0), prefix("") {
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

Server::~Server() {
  close();
}

void Server::setPrefix(string prefix_) {
  prefix = prefix_;
}

void Server::closeHandler(struct mg_connection *conn) {
  struct mg_request_info *request_info = mg_get_request_info(conn);
  assert(request_info != NULL);
  Server *me = (Server *)(request_info->user_data);
  assert(me != NULL);

  if (me->userCloseHandler)
    me->userCloseHandler(conn);
}

void Server::close() {
  if (context) {
    mg_stop(context);
    context = 0;
  }
}

Callback Server::route(string httpMethod, string url, Callback callback) {
  string key = httpMethod + ":" + prefix + url + "/?";
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
    key = string(request_info->request_method) + ":" + string(request_info->uri);
    if (regex_match(key, matches, regex(it->first))) {
      Request *request = new Request(conn);
      Response *response = new Response();
      request->matches = matches;
      it->second(*request, *response);
      data = response->getData();
      mg_write(conn, data.c_str(), data.size());
      return 1;
    }
  }
  return 0;
}

int Request::getCookie(const string &cookieName, string &cookieValue) {
  // Maximum cookie length as per microsoft is 4096. http://msdn.microsoft.com/en-us/library/ms178194.aspx
  char _cookieValue[4096];
  const char *cookie = mg_get_header(connection, "Cookie");
  int lRead = mg_get_cookie(cookie, cookieName.c_str(), _cookieValue, sizeof(_cookieValue));
  cookieValue.clear();
  cookieValue.append(_cookieValue);
  return lRead;
}

const char *Request::getHeader(const string &headerName) {
  return mg_get_header(connection, headerName.c_str());
}

const char *Request::getPostData() {
  return postData;
}

bool Request::getParam(const char *name, string &dst, size_t occurrence) {
  auto i = values.find(name);
  if (i != values.end()) {
    dst = i->second[occurrence];
    return true;
  }
  return false;
}

vector<string> Request::getParamArray(const char *name) {
  return values[name];
}

bool Request::getQueryParam(const char *name, string &dst, size_t occurrence) {
  auto i = query.find(name);
  if (i != query.end()) {
    dst = i->second[occurrence];
    return true;
  }
  return false;
}

vector<string> Request::getQueryParamArray(const char *name) {
  return query[name];
}

map<string, vector<string> > parse_values(string data, string boundary, bool multipart = false) {
  map<string, vector<string> > result;
  string block, block_header, block_body, delim;
  smatch field_name;
  regex name_regex;
  int start = 0, end = 0, delim_pos = 0, type;
  if (multipart) {
    data = "\r\n" + data;
    boundary = "\r\n--" + boundary;
    name_regex = regex("name=\"([^\"]+)\"");
    delim = "\r\n\r\n";
  } else {
    name_regex = regex("(.+)");
    delim = "=";
  }
  while (start >= 0) {
    end = data.find(boundary, start);
    if (end == start) {
      start += boundary.size();
      end = data.find(boundary, start);
    }
    block = data.substr(start, end - start);
    start = end;
    delim_pos = block.find(delim);
    if (delim_pos < 0)
      continue;
    block_header = block.substr(0, delim_pos);
    block_body = block.substr(delim_pos + delim.size());
    if (regex_search(block_header, field_name, name_regex)) {
      if (result.find(field_name[1]) == result.end())
        result[field_name[1]] = vector<string>();
      if (multipart)
        Util::urlDecode(std::string(block_body), block_body);
      result[field_name[1]].push_back(block_body);
    }
  }
  return result;
}

Request::Request(struct mg_connection *connection_)
    : connection(connection_), request_info(mg_get_request_info(connection_)), postData(NULL), matches() {
  // postData
  unsigned long con_len;
  const char *con_len_str = mg_get_header(connection, "Content-Length");
  if (con_len_str) {
    con_len = atoi(con_len_str);
    if (con_len > 0) {
      postData = (char *)calloc(con_len, sizeof(char));
      if (postData != NULL)
        mg_read(connection, postData, con_len);  // malloc may fail for huge requests
    }
  }
  const char* temp = mg_get_request_info(connection)->query_string;
  if(temp!=NULL)
    query = parse_values(temp, "&");
  
  temp = mg_get_header(connection, "Content-Type");
  if(temp!=NULL){
    string content_type = temp;
    values = parse_values(string(postData).substr(0, con_len), content_type.substr(content_type.find("boundary=") + 9),
                          content_type.find("multipart/form-data") >= 0);
  }
}

Request::~Request() {
  if (postData) {
    free(postData);
    postData = NULL;
  }
}

smatch Request::getMatches() {
  return matches;
}

Response::Response() : code(Response::codes::OK), headers() {};

void Response::setHeader(string key, string value) {
  headers[key] = value;
}

bool Response::hasHeader(string key) {
  return headers.find(key) != headers.end();
}

string Response::getData() {
  string body = getBody();
  ostringstream data;
  data << "HTTP/1.0 " << code << "\r\n";
  if (!hasHeader("Content-Length")) {
    ostringstream length;
    length << body.size();
    setHeader("Content-Length", length.str());
  }
  map<string, string>::iterator it;
  for (it = headers.begin(); it != headers.end(); it++) {
    data << (*it).first << ": " << (*it).second << "\r\n";
  }
  data << "\r\n";
  data << body;
  return data.str();
}

void Response::setCookie(string key, string value) {
  ostringstream definition;
  definition << key << "=" << value << "; path=/";
  setHeader("Set-cookie", definition.str());
}

void Response::setCode(int code_) {
  code = code_;
}

string Response::getBody() {
  return this->str();
}

}  // namespace Civetta
