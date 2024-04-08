#ifndef GOOGLE_EVENTS_API_H
#define GOOGLE_EVENTS_API_H

#include <string>
#include <utility>
#include <vector>

#include "APIBase.h"
#include "GoogleTokens.h"
#include "ProfileManager.h"

class GoogleEventsAPI : public ApiBase {
private:
   GoogleTokens googleTokens;
   ProfileManager profileManager;
   std::vector<std::pair<std::string, std::string>> calendarList;

public:
   GoogleEventsAPI();
   GoogleEventsAPI(GoogleTokens googleTokens) : googleTokens(googleTokens){};
   std::vector<std::pair<std::string, std::string>> fetchCalendarList();
   void list() override;
   void list(int daysBefore, int daysAfter);
   void add() override;
   void insertEvent(std::string title = "", std::string startDateTime = "",
                  std::string endDateTime = "");
};
#endif