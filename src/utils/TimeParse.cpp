#include "utils/TimeParse.h"

#include <chrono>
#include <iomanip>
#include <sstream>
#include <cmath>

std::tm TimeParse::getCurrentDateTime() {
    std::time_t t = std::time(nullptr);
    std::tm *now = std::gmtime(&t);
    return *now;
}

std::tm TimeParse::getShiftedDateTime(int days) {
    std::time_t t = std::time(nullptr) + days * 24 * 60 * 60; // Shift by days
    std::tm *shifted = std::gmtime(&t);
    return *shifted;
}


time_t TimeParse::getLocalTimezoneOffset() {
    auto now = std::chrono::system_clock::now();
    std::time_t t_now = std::chrono::system_clock::to_time_t(now);

    std::tm utc_tm = *std::gmtime(&t_now);
    std::tm local_tm = *std::localtime(&t_now);

    return std::mktime(&local_tm) - std::mktime(&utc_tm); // Return the difference in seconds and use +, - to indicate the timezone
}

std::string TimeParse::castToRFC3339(const std::tm &dateTime) {
    time_t timezoneOffset = getLocalTimezoneOffset();
    std::ostringstream oss;

    // Format the date-time string in UTC
    oss << std::put_time(&dateTime, "%Y-%m-%dT%H:%M:00");
    // Format the timezone offset, assuming your system's timezone is what you want to express
    oss << (timezoneOffset >= 0 ? "+" : "-")
        << std::setw(2) << std::setfill('0') << std::abs(timezoneOffset) / 3600
        << ":00";

    return oss.str();
}

std::tm TimeParse::parseRFC3339(const std::string &rfc3339Str) {
    std::tm parsed;
    std::istringstream ss(rfc3339Str);

    ss >> std::get_time(&parsed, "%Y-%m-%dT%H:%M");

    return parsed;
}