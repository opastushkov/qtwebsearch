#ifndef HTTPREGEX
#define HTTPREGEX

#define LETTERS "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz"
#define NUMBERS "0123456789"
#define SYMBOLS R"(\-._~:\/?#\[\]@!$&'()*+,;=)"
#define ALLOWED_URL_SYMS LETTERS NUMBERS SYMBOLS
#define URL_REGEX "https?://[" ALLOWED_URL_SYMS "]*"

#endif // HTTPREGEX

