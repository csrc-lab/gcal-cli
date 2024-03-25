#ifndef TIMEPARSE_H
#define TIMEPARSE_H

#include <ctime>
#include <string>

class TimeParse {
public:
    static std::tm getCurrentDateTime();
    static std::tm getShiftedDateTime(int days);
    static std::string castToRFC3339(const std::tm &dateTime);
    static std::tm parseRFC3339(const std::string &rfc3339Str);
private:
    static time_t getLocalTimezoneOffset(); // get the local timezone offset in seconds
};

#endif