#include <civetta/Util.h>

namespace Civetta {

void Util::urlDecode(const std::string &src, std::string &dst,
                     bool is_form_url_encoded) {
  urlDecode(src.c_str(), src.length(), dst, is_form_url_encoded);
}

void Util::urlDecode(const char *src, std::string &dst,
                     bool is_form_url_encoded) {
  urlDecode(src, strlen(src), dst, is_form_url_encoded);
}

void Util::urlDecode(const char *src, size_t src_len, std::string &dst,
                     bool is_form_url_encoded) {
  int i, j, a, b;
#define HEXTOI(x) (isdigit(x) ? x - '0' : x - 'W')

  dst.clear();
  for (i = j = 0; i < (int)src_len; i++, j++) {
    if (src[i] == '%' && i < (int)src_len - 2 &&
        isxdigit(*(const unsigned char *)(src + i + 1)) &&
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
}

void Util::urlEncode(const std::string &src, std::string &dst, bool append) {
  urlEncode(src.c_str(), src.length(), dst, append);
}

void Util::urlEncode(const char *src, std::string &dst, bool append) {
  urlEncode(src, strlen(src), dst, append);
}

void Util::urlEncode(const char *src, size_t src_len, std::string &dst,
                     bool append) {
  static const char *dont_escape = "._-$,;~()";
  static const char *hex = "0123456789abcdef";

  if (!append)
    dst.clear();

  for (; src_len > 0; src++, src_len--) {
    if (isalnum(*(const unsigned char *)src) ||
        strchr(dont_escape, *(const unsigned char *)src) != NULL) {
      dst.push_back(*src);
    } else {
      dst.push_back('%');
      dst.push_back(hex[(*(const unsigned char *)src) >> 4]);
      dst.push_back(hex[(*(const unsigned char *)src) & 0xf]);
    }
  }
}
}