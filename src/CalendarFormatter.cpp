#include <CalendarFormatter.h>
#include <utils/TimeParse.h>

#include <algorithm>
#include <ctime>
#include <iomanip>
#include <iostream>
#include <map>
#include <sstream>
#include <string>
#include <tuple>
#include <vector>

const std::string ORANGE = "\033[38;5;208m";
const std::string RESET = "\033[0m";

void CalendarFormatter::dayView(
    const std::map<std::tm,
                   std::vector<std::tuple<std::tm, std::tm, std::string>>,
                   TmComparator>& events) {
    int dateColWidth = 15;
    int timeColWidth = 15;
    int eventColWidth = 30;
    std::cout << std::left << std::setw(dateColWidth) << "Date"
              << std::setw(timeColWidth) << "Time" << std::setw(eventColWidth)
              << "Events" << std::endl;

    std::cout << std::string(dateColWidth + timeColWidth + eventColWidth, '-')
              << std::endl;

    for (auto& event : events) {
        const auto& date = event.first;
        auto dayEvents = event.second;  // Make a copy that can be sorted.

        std::sort(dayEvents.begin(), dayEvents.end(),
                  [](const auto& lhs, const auto& rhs) {
                      return std::tie(std::get<0>(lhs).tm_hour,
                                      std::get<0>(lhs).tm_min) <
                             std::tie(std::get<0>(rhs).tm_hour,
                                      std::get<0>(rhs).tm_min);
                  });

        std::stringstream dateStream;
        dateStream << std::put_time(&date, "%Y-%m-%d");
        std::string dateStr = dateStream.str();
        for (size_t i = 0; i < dayEvents.size(); ++i) {
            const auto& [startTime, endTime, eventName] = dayEvents[i];

            std::stringstream startTimeStr, endTimeStr;
            startTimeStr << std::put_time(&startTime, "%H:%M");
            endTimeStr << std::put_time(&endTime, "%H:%M");

            std::string timeStr = startTimeStr.str() + " - " + endTimeStr.str();

            if (i == 0) {
                if (TimeParse::isToday(date)) {
                    std::cout << std::left
                              << std::setw(dateColWidth + ORANGE.length() +
                                           RESET.length())
                              << ORANGE + dateStr + RESET
                              << std::setw(timeColWidth) << timeStr
                              << std::setw(eventColWidth) << eventName
                              << std::endl;
                } else {
                    std::cout << std::left << std::setw(dateColWidth) << dateStr
                              << std::setw(timeColWidth) << timeStr
                              << std::setw(eventColWidth) << eventName
                              << std::endl;
                }
            } else {
                std::cout << std::left << std::setw(dateColWidth) << ""
                          << std::setw(timeColWidth) << timeStr
                          << std::setw(eventColWidth) << eventName << std::endl;
            }
        }
        std::cout << std::string(dateColWidth + timeColWidth + eventColWidth,
                                 '-')
                  << std::endl;
    }

    return;
}

void CalendarFormatter::dayView(
    const std::map<std::tm, std::vector<std::string>, TmComparator>& tasks) {
    int dateColWidth = 20;
    int taskColWidth = 30;
    std::cout << std::left << std::setw(dateColWidth) << "Date"
              << std::setw(taskColWidth) << "Tasks" << std::endl;
    std::cout << std::string(dateColWidth + taskColWidth, '-') << std::endl;

    for (const auto& task : tasks) {
        const auto& date = task.first;
        const auto& dayEvents = task.second;

        char dateStr[11];
        std::strftime(dateStr, sizeof(dateStr), "%Y-%m-%d", &date);

        if (!dayEvents.empty()) {
            if (TimeParse::isToday(date)) {
                std::cout << std::left
                          << std::setw(dateColWidth + ORANGE.length() +
                                       RESET.length())
                          << ORANGE + dateStr + RESET << std::setw(taskColWidth)
                          << dayEvents[0] << std::endl;
            } else {
                std::cout << std::left << std::setw(dateColWidth) << dateStr
                          << std::setw(taskColWidth) << dayEvents[0]
                          << std::endl;
            }
            for (size_t i = 1; i < dayEvents.size(); ++i) {
                std::cout << std::left << std::setw(dateColWidth) << ""
                          << std::setw(taskColWidth) << dayEvents[i]
                          << std::endl;
            }
            std::cout << std::string(dateColWidth + taskColWidth, '-')
                      << std::endl;
        } else {
            std::cout << std::left << std::setw(dateColWidth) << dateStr
                      << std::setw(taskColWidth) << "No Tasks" << std::endl;
        }
    }
}
