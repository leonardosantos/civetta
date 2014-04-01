#ifndef _CIVETTA_UTIL_H_
#define _CIVETTA_UTIL_H_

#include <string>
#include <civetta/Common.h>

namespace Civetta {

class CIVETTA_EXPORT Util {
public:
  /**
   * urlDecode(const char *, size_t, std::string &, bool)
   *
   * @param src buffer to be decoded
   * @param src_len length of buffer to be decoded
   * @param dst destination string
   * @is_form_url_encoded true if form url encoded
   *       form-url-encoded data differs from URI encoding in a way that it
   *       uses '+' as character for space, see RFC 1866 section 8.2.1
   *       http://ftp.ics.uci.edu/pub/ietf/html/rfc1866.txt
   */
  static void urlDecode(const char *src, size_t src_len, std::string &dst,
                        bool is_form_url_encoded = true);

  /**
   * urlDecode(const std::string &, std::string &, bool)
   *
   * @param src string to be decoded
   * @param dst destination string
   * @is_form_url_encoded true if form url encoded
   *       form-url-encoded data differs from URI encoding in a way that it
   *       uses '+' as character for space, see RFC 1866 section 8.2.1
   *       http://ftp.ics.uci.edu/pub/ietf/html/rfc1866.txt
   */
  static void urlDecode(const std::string &src, std::string &dst,
                        bool is_form_url_encoded = true);

  /**
   * urlDecode(const char *, std::string &, bool)
   *
   * @param src buffer to be decoded (0 terminated)
   * @param dst destination string
   * @is_form_url_encoded true if form url encoded
   *       form-url-encoded data differs from URI encoding in a way that it
   *       uses '+' as character for space, see RFC 1866 section 8.2.1
   *       http://ftp.ics.uci.edu/pub/ietf/html/rfc1866.txt
   */
  static void urlDecode(const char *src, std::string &dst,
                        bool is_form_url_encoded = true);

  /**
   * urlEncode(const char *, size_t, std::string &, bool)
   *
   * @param src buffer to be encoded
   * @param src_len length of buffer to be decoded
   * @param dst destination string
   * @append true if string should not be cleared before encoding.
   */
  static void urlEncode(const char *src, size_t src_len, std::string &dst,
                        bool append = false);

  /**
   * urlEncode(const char *, size_t, std::string &, bool)
   *
   * @param src buffer to be encoded (0 terminated)
   * @param dst destination string
   * @append true if string should not be cleared before encoding.
   */
  static void urlEncode(const char *src, std::string &dst, bool append = false);

  /**
   * urlEncode(const std::string &, std::string &, bool)
   *
   * @param src buffer to be encoded
   * @param dst destination string
   * @append true if string should not be cleared before encoding.
   */
  static void urlEncode(const std::string &src, std::string &dst,
                        bool append = false);
};
}
#endif // _CIVETTA_UTIL_H_
