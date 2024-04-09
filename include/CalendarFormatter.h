#ifndef CALENDAR_FORMATTER_H
#define CALENDAR_FORMATTER_H

#include <utils/TimeParse.h>

#include <ctime>
#include <map>
#include <string>
#include <tuple>
#include <vector>

class CalendarFormatter {
   public:
    static void dayView(
        const std::map<std::tm,
                       std::vector<std::tuple<std::tm, std::tm, std::string>>,
                       TmComparator>& events);
    static void dayView(
        const std::map<std::tm, std::vector<std::string>, TmComparator>& tasks);
};

#endif  // CALENDAR_FORMATTER_H