#ifndef GOOGLE_EVENTS_API_H
#define GOOGLE_EVENTS_API_H

#include "APIBase.h"
#include "TokenManager.h"
#include <string>

class GoogleEventsAPI : public ApiBase {
    std::string token;
public:
    GoogleEventsAPI();
    void list() override;
    void create() override;
};
#endif