#ifndef BASE64_H
#define BASE64_H

#include <string>

std::string base64_encode(const std::string &in);
std::string base64_decode(const std::string &in);

#endif // BASE64_H