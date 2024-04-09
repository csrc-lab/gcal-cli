#include "GoogleEventsAPI.h"

#include <cpr/cpr.h>

#include <iostream>
#include <nlohmann/json.hpp>
#include <unordered_map>
#include <vector>

#include "CalendarFormatter.h"
#include "ConfigManager.h"
#include "ProfileManager.h"
#include "utils/TimeParse.h"

GoogleEventsAPI::GoogleEventsAPI() {
    profileManager = ProfileManager();
    googleTokens = profileManager.getTokens();
    calendarList = profileManager.getCalendarList();
}

void printEvents(std::vector<nlohmann::json> items) {
    for (int i = 1; i <= items.size(); i++) {
        std::string startRFC = items[i - 1]["start"]["dateTime"];
        std::string endRFC = items[i - 1]["end"]["dateTime"];
        std::tm start = TimeParse::parseRFC3339(startRFC);
        std::tm end = TimeParse::parseRFC3339(endRFC);
        std::cout << i << ". " << std::put_time(&start, "%Y-%m-%d %H:%M")
                  << " - " << std::put_time(&end, "%H:%M") << " : "
                  << items[i - 1]["summary"] << std::endl;
    }
}

std::vector<std::pair<std::string, std::string>>
GoogleEventsAPI::fetchCalendarList() {
    std::string apiUrl =
        "https://www.googleapis.com/calendar/v3/users/me/calendarList";
    cpr::Response r =
        cpr::Get(cpr::Url{apiUrl},
                 cpr::Header{{"Authorization", "Bearer " + googleTokens.token},
                             {"Accept", "application/json"}});
    if (r.status_code == 200) {
        nlohmann::json j = nlohmann::json::parse(r.text);
        std::vector<nlohmann::json> items =
            j["items"].get<std::vector<nlohmann::json>>();
        std::vector<std::pair<std::string, std::string>> calendarList;
        for (nlohmann::json item : items) {
            std::string id = item["id"];
            std::string summary = item["summary"];
            calendarList.push_back(std::make_pair(id, summary));
        }
        return calendarList;
    } else if (r.status_code == 401) {
        std::cerr << "Error: Unauthorized" << std::endl;
        ConfigManager::refreshConfiguration();
        googleTokens = ProfileManager().getTokens();
        return fetchCalendarList();
    } else {
        std::cerr << "Error: " << r.status_code << std::endl;
        std::cerr << r.text << std::endl;
        return {};
    }
}

void GoogleEventsAPI::list() { list(7, 7); }
void GoogleEventsAPI::list(int daysBefore, int daysAfter, std::string keyword) {
    std::string rfcStartDay =
        TimeParse::castToRFC3339(TimeParse::getShiftedDateTime(-daysBefore));
    std::string rfcEndDay =
        TimeParse::castToRFC3339(TimeParse::getShiftedDateTime(daysAfter));

    // check whether to refresh the configuration first
    if (isRefreshConfigNeeded(rfcStartDay, rfcEndDay)) {
        ConfigManager::refreshConfiguration();
        googleTokens = ProfileManager().getTokens();
    }

    std::vector<std::tuple<std::string, std::string, cpr::AsyncResponse>>
        container{};  // calendarId, calendarName, response
    int eventCount = 0;

    cpr::Header headers = {{"Authorization", "Bearer " + googleTokens.token},
                           {"Accept", "application/json"}};
    cpr::Parameters params = {{"timeMin", rfcStartDay},
                              {"timeMax", rfcEndDay},
                              {"orderBy", "startTime"},
                              {"singleEvents", "true"},
                              {"timeZone", profileManager.getTimezone()}};
    if (!keyword.empty()) {
        params.Add({"q", keyword});
    }

    for (auto calendar : profileManager.getCalendarList()) {
        std::string apiUrl =
            "https://www.googleapis.com/calendar/v3/calendars/" +
            calendar.first + "/events";
        container.push_back(
            std::make_tuple(calendar.first, calendar.second,
                            cpr::GetAsync(cpr::Url{apiUrl}, headers, params)));
    }

    std::map<std::tm, std::vector<std::tuple<std::tm, std::tm, std::string>>,
             TmComparator>
        eventsByDate;
    for (auto& [calendarId, calendarName, res] : container) {
        cpr::Response r = res.get();
        if (r.status_code == 200) {
            nlohmann::json j = nlohmann::json::parse(r.text);
            std::vector<nlohmann::json> rawItems =
                j["items"].get<std::vector<nlohmann::json>>();
            // Filter out the events with status confirmed only
            std::vector<nlohmann::json> items;
            std::copy_if(rawItems.begin(), rawItems.end(),
                         std::back_inserter(items), [](nlohmann::json item) {
                             return item["status"] == "confirmed";
                         });
            eventCount += items.size();

            // add events to eventsByDate
            for (nlohmann::json item : items) {
                std::string startRFC = item["start"]["dateTime"];
                std::string endRFC = item["end"]["dateTime"];
                std::tm start = TimeParse::parseRFC3339(startRFC);
                std::tm end = TimeParse::parseRFC3339(endRFC);
                std::tm dateKey = start;
                dateKey.tm_hour = 0;
                dateKey.tm_min = 0;
                dateKey.tm_sec = 0;
                dateKey.tm_isdst = -1;

                eventsByDate[dateKey].push_back(
                    std::make_tuple(start, end, item["summary"]));
            }
        } else if (r.status_code == 401) {
            std::cerr << "Error: Refresh has performed but still get "
                         "Unauthorized Error"
                      << std::endl;
        } else {
            std::cerr << "Error: " << r.status_code << std::endl;
            std::cerr << r.text << std::endl;
        }
    }
    std::cout << "Below are the " << eventCount << " tasks in the last "
              << daysBefore << " days and the following " << daysAfter
              << " days:" << std::endl
              << std::endl;

    CalendarFormatter::dayView(eventsByDate);
}

bool GoogleEventsAPI::isRefreshConfigNeeded(std::string rfcStartDay,
                                            std::string rfcEndDay) {
    auto googleTokens = profileManager.getTokens();
    std::string apiUrl =
        "https://www.googleapis.com/calendar/v3/calendars/primary/events";
    cpr::Response r =
        cpr::Get(cpr::Url{apiUrl},
                 cpr::Header{{"Authorization", "Bearer " + googleTokens.token},
                             {"Accept", "application/json"}},
                 cpr::Parameters{{"timeMin", rfcStartDay},
                                 {"timeMax", rfcEndDay},
                                 {"orderBy", "startTime"},
                                 {"singleEvents", "true"}});
    if (r.status_code == 401) {
        return true;
    }
    return false;
}
void GoogleEventsAPI::add() { insertEvent(); }

void GoogleEventsAPI::insertEvent(std::string title, std::string startDateTime,
                                  std::string endDateTime) {
    std::string calendarId = "primary";  // default calendar
    std::string timeZone = profileManager.getTimezone();

    if (title.empty()) {
        std::cout << "Enter the title of the event: ";
        std::getline(std::cin >> std::ws, title);  // ignore leading whitespace
    }
    if (startDateTime.empty()) {
        std::cout << "Enter the start date and time (YYYY-MM-DD HH:MM): ";
        std::getline(std::cin >> std::ws, startDateTime);
        std::string date = startDateTime.substr(0, 10);
        std::string time = startDateTime.substr(11, 5);
        startDateTime = date + "T" + time;
        startDateTime.append(":00");
    }
    if (endDateTime.empty()) {
        std::cout << "Enter the end date and time (YYYY-MM-DD HH:MM): ";
        std::getline(std::cin >> std::ws, endDateTime);
        std::string date = endDateTime.substr(0, 10);
        std::string time = endDateTime.substr(11, 5);
        endDateTime = date + "T" + time;
        endDateTime.append(":00");
    }

    std::string apiUrl = "https://www.googleapis.com/calendar/v3/calendars/" +
                         calendarId + "/events";
    nlohmann::json j = {
        {"summary", title},
        {"start", {{"dateTime", startDateTime}, {"timeZone", timeZone}}},
        {"end", {{"dateTime", endDateTime}, {"timeZone", timeZone}}}};

    cpr::Response r =
        cpr::Post(cpr::Url{apiUrl},
                  cpr::Header{{"Authorization", "Bearer " + googleTokens.token},
                              {"Accept", "application/json"},
                              {"Content-Type", "application/json"}},
                  cpr::Body{j.dump()});
    if (r.status_code == 200 || r.status_code == 201) {
        std::cout << "Event added successfully" << std::endl;
    } else if (r.status_code == 401) {
        std::cerr << "Error: Unauthorized" << std::endl;
        ConfigManager::refreshConfiguration();
        googleTokens = profileManager.getTokens();
        insertEvent(title, startDateTime, endDateTime);
    } else {
        std::cerr << "Error: " << r.status_code << std::endl;
        std::cerr << r.text << std::endl;
    }
}