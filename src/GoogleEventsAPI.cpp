#include "GoogleEventsAPI.h"

#include <cpr/cpr.h>
#include "ConfigManager.h"
#include <iostream>

GoogleEventsAPI::GoogleEventsAPI() {
    auto tokenManager = TokenManager();
    googleTokens = tokenManager.getTokens();
}

void GoogleEventsAPI::list() {
    std::string calendarId = "primary";
    std::string apiUrl = "https://www.googleapis.com/calendar/v3/calendars/" + calendarId + "/events";


    cpr::Response r = cpr::Get(cpr::Url{apiUrl}, cpr::Header{
        {"Authorization", "Bearer " + googleTokens.token},
        {"Accept", "application/json"}
    });
    if (r.status_code == 200) {
        std::cout << r.text << std::endl;
    } else if (r.status_code == 401) {
        std::cerr << "Error: Unauthorized" << std::endl;
        ConfigManager::refreshConfiguration();
        googleTokens = TokenManager().getTokens();
    } else {
        std::cerr << "Error: " << r.status_code << std::endl;
    }
}

void GoogleEventsAPI::add() {
    std::cout << "Creating event" << std::endl;
}