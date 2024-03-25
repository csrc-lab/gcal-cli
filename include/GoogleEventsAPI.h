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
    void add() override;
};
#endif