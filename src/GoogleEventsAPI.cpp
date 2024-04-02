#include "GoogleEventsAPI.h"

#include <cpr/cpr.h>

#include <iostream>
#include <nlohmann/json.hpp>
#include <vector>

#include "ConfigManager.h"
#include "utils/TimeParse.h"

GoogleEventsAPI::GoogleEventsAPI() {
    ProfileManager profileManager;
    googleTokens = profileManager.getTokens();
}

void GoogleEventsAPI::list() { list(7, 7); }
void GoogleEventsAPI::list(int daysBefore, int daysAfter) {
    std::string rfcStartDay =
        TimeParse::castToRFC3339(TimeParse::getShiftedDateTime(-daysBefore));
    std::string rfcEndDay =
        TimeParse::castToRFC3339(TimeParse::getShiftedDateTime(daysAfter));

    std::string calendarId = "primary";
    std::string apiUrl = "https://www.googleapis.com/calendar/v3/calendars/" +
                         calendarId + "/events";

    cpr::Response r =
        cpr::Get(cpr::Url{apiUrl},
                 cpr::Header{{"Authorization", "Bearer " + googleTokens.token},
                             {"Accept", "application/json"}},
                 cpr::Parameters{{"timeMin", rfcStartDay},
                                 {"timeMax", rfcEndDay},
                                 {"orderBy", "startTime"},
                                 {"singleEvents", "true"}});
    if (r.status_code == 200) {
        nlohmann::json j = nlohmann::json::parse(r.text);
        std::vector<nlohmann::json> rawItems =
            j["items"].get<std::vector<nlohmann::json>>();
        // Filter out the events with status confirmed only
        std::vector<nlohmann::json> items;
        std::copy_if(
            rawItems.begin(), rawItems.end(), std::back_inserter(items),
            [](nlohmann::json item) { return item["status"] == "confirmed"; });

        std::cout << "Below are the " << items.size() << " tasks in the last "
                  << daysBefore << " days and the following " << daysAfter
                  << " days:" << std::endl;

        for (int i = 1; i <= items.size(); i++) {
            std::string startRFC = items[i - 1]["start"]["dateTime"];
            std::string endRFC = items[i - 1]["end"]["dateTime"];
            std::tm start = TimeParse::parseRFC3339(startRFC);
            std::tm end = TimeParse::parseRFC3339(endRFC);
            std::cout << i << ". " << std::put_time(&start, "%Y-%m-%d %H:%M")
                      << " - " << std::put_time(&end, "%H:%M") << " : "
                      << items[i - 1]["summary"] << std::endl;
        }
    } else if (r.status_code == 401) {
        std::cerr << "Error: Unauthorized" << std::endl;
        ConfigManager::refreshConfiguration();
        googleTokens = ProfileManager().getTokens();
        list();
    } else {
        std::cerr << "Error: " << r.status_code << std::endl;
        std::cerr << r.text << std::endl;
    }
}

void GoogleEventsAPI::add() { insertEvent(); }

void GoogleEventsAPI::insertEvent(std::string title, std::string startDateTime,
                                  std::string endDateTime) {
    std::string calendarId = "primary";  // default calendar
    std::string timeZone = "Asia/Taipei";

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
        googleTokens = ProfileManager().getTokens();
        insertEvent(title, startDateTime, endDateTime);
    } else {
        std::cerr << "Error: " << r.status_code << std::endl;
        std::cerr << r.text << std::endl;
    }
}