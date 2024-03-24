#include "GoogleEventsAPI.h"
#include <iostream>

GoogleEventsAPI::GoogleEventsAPI() {
    TokenManager tokenManager;
    token = tokenManager.getToken();
}

void GoogleEventsAPI::list() {
    std::cout << "Listing events" << std::endl;
}

void GoogleEventsAPI::create() {
    std::cout << "Creating event" << std::endl;
}