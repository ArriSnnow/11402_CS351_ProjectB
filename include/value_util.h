#pragma once
#include "csv_parser.h"
#include <string>
#include <sstream>
#include <stdexcept>
#include <iomanip>

// Helpers for working with the variant Value type (int, double, bool, string).
// Header-only so it can be dropped in without touching CMake source lists.
namespace valueutil {

// Render a Value as text (used by the CLI and aggregate output).
inline std::string toString(const Value& v) {
    if (std::holds_alternative<int>(v))         return std::to_string(std::get<int>(v));
    if (std::holds_alternative<bool>(v))        return std::get<bool>(v) ? "true" : "false";
    if (std::holds_alternative<std::string>(v)) return std::get<std::string>(v);
    // double: keep meaningful precision, then trim trailing zeros for tidy display
    std::ostringstream oss;
    oss << std::setprecision(12) << std::get<double>(v);
    std::string s = oss.str();
    if (s.find('.') != std::string::npos) {
        s.erase(s.find_last_not_of('0') + 1);
        if (!s.empty() && s.back() == '.') s.pop_back();
    }
    return s;
}

// Is this Value numeric (int or double)?
inline bool isNumeric(const Value& v) {
    return std::holds_alternative<int>(v) || std::holds_alternative<double>(v);
}

// Numeric view of a Value. bool counts as 0/1. Throws if not convertible.
inline double toNumber(const Value& v) {
    if (std::holds_alternative<int>(v))    return static_cast<double>(std::get<int>(v));
    if (std::holds_alternative<double>(v)) return std::get<double>(v);
    if (std::holds_alternative<bool>(v))   return std::get<bool>(v) ? 1.0 : 0.0;
    throw std::runtime_error("Value is not numeric: " + toString(v));
}

// Three-way comparison: returns <0, 0, or >0.
// Numbers compare numerically (int/double mix allowed); strings lexicographically;
// bools as 0/1. Mismatched non-numeric types fall back to comparing their text.
inline int compare(const Value& a, const Value& b) {
    if (isNumeric(a) && isNumeric(b)) {
        double da = toNumber(a), db = toNumber(b);
        if (da < db) return -1;
        if (da > db) return 1;
        return 0;
    }
    if (std::holds_alternative<bool>(a) && std::holds_alternative<bool>(b)) {
        return (std::get<bool>(a) ? 1 : 0) - (std::get<bool>(b) ? 1 : 0);
    }
    if (std::holds_alternative<std::string>(a) && std::holds_alternative<std::string>(b)) {
        return std::get<std::string>(a).compare(std::get<std::string>(b));
    }
    // Last resort: compare textual forms so ORDER BY never throws on mixed columns.
    return toString(a).compare(toString(b));
}

// SQL-style LIKE: '%' matches any run of characters, '_' matches one character.
inline bool like(const std::string& text, const std::string& pattern) {
    size_t t = 0, p = 0, star = std::string::npos, mark = 0;
    while (t < text.size()) {
        if (p < pattern.size() && (pattern[p] == '_' || pattern[p] == text[t])) {
            ++t; ++p;
        } else if (p < pattern.size() && pattern[p] == '%') {
            star = p++; mark = t;
        } else if (star != std::string::npos) {
            p = star + 1; t = ++mark;
        } else {
            return false;
        }
    }
    while (p < pattern.size() && pattern[p] == '%') ++p;
    return p == pattern.size();
}

} // namespace valueutil
