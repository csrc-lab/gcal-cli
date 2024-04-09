#ifndef TIMEPARSE_H
#define TIMEPARSE_H

#include <ctime>
#include <string>
static struct TmComparator {
    bool operator()(const std::tm& lhs, const std::tm& rhs) const {
        if (lhs.tm_year != rhs.tm_year) return lhs.tm_year < rhs.tm_year;
        if (lhs.tm_mon != rhs.tm_mon) return lhs.tm_mon < rhs.tm_mon;
        if (lhs.tm_mday != rhs.tm_mday) return lhs.tm_mday < rhs.tm_mday;
        if (lhs.tm_hour != rhs.tm_hour) return lhs.tm_hour < rhs.tm_hour;
        if (lhs.tm_min != rhs.tm_min) return lhs.tm_min < rhs.tm_min;
        return lhs.tm_sec < rhs.tm_sec;
    }
};

class TimeParse {
   public:
    static std::tm getCurrentDateTime();
    static std::tm getShiftedDateTime(int days);
    static std::string castToRFC3339(const std::tm& dateTime);
    static std::tm parseRFC3339(const std::string& rfc3339Str);
    static bool isToday(const std::tm& date);

   private:
    static time_t
    getLocalTimezoneOffset();  // get the local timezone offset in seconds
};

#endif