#ifndef GOOGLE_EVENTS_API_H
#define GOOGLE_EVENTS_API_H

#include "APIBase.h"
#include "TokenManager.h"
#include <string>

class GoogleEventsAPI : public ApiBase {
private:
    GoogleTokens googleTokens;
public:
    GoogleEventsAPI();
    void list() override;
    void list(int daysBefore, int daysAfter);
    void add() override;
    void insertEvent(std::string title = "", std::string startDateTime = "", std::string endDateTime = "");
};
#endif