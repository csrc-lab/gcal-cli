#ifndef GOOGLE_EVENTS_API_H
#define GOOGLE_EVENTS_API_H

#include <string>

#include "APIBase.h"
#include "ProfileManager.h"

class GoogleEventsAPI : public ApiBase {
   private:
    GoogleTokens googleTokens;

   public:
    GoogleEventsAPI();
    void list() override;
    void list(int daysBefore, int daysAfter);
    void add() override;
    void insertEvent(std::string title = "", std::string startDateTime = "",
                     std::string endDateTime = "");
};
#endif