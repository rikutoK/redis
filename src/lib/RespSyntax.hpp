#ifndef RESPSYNTAX_HPP
#define RESPSYNTAX_HPP

#include <string>

inline constexpr std::string delimiter = "\r\n";
inline constexpr char array_prefix = '*';
inline constexpr char bulk_string_prefix = '$';
inline constexpr char simple_string_prefix = '+';
inline constexpr std::string null_bulk_string = "$-1\r\n";

#endif
