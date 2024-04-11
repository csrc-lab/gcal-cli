#ifndef GOOGLEOAUTH_H
#define GOOGLEOAUTH_H

#include <string>
#include <vector>

#include "GoogleTokens.h"

class GoogleOauth {
   private:
    std::string clientSecret;
    std::string clientId;
    std::string redirectUri = "http://localhost:9999";
    std::vector<std::string> scopes = {
        "https://www.googleapis.com/auth/calendar",
        "https://www.googleapis.com/auth/tasks"};
    std::string authUrl = "https://accounts.google.com/o/oauth2/auth";

    std::string token;
    std::string refreshToken;

   public:
    GoogleOauth(GoogleTokens &tokens);
    GoogleOauth(const std::string &credPath);
    void showCredential();
    void setCredential(const std::string &cred);
    std::string getAuthorizationUrl();
    GoogleTokens getAccessToken(const std::string &code);
    std::string getRefreshToken();
    std::string getClientId();
    std::string getClientSecret();
    GoogleTokens refreshTokens();
};

#endif