#ifndef CREDS_H
#define CREDS_H

#include <string>
#include <vector>

class GoogleOauth {
   private:
    std::string clientSecret;
    std::string clientId;
    std::string redirectUri = "urn:ietf:wg:oauth:2.0:oob";
    std::vector<std::string> scopes = {
        "https://www.googleapis.com/auth/calendar",
        "https://www.googleapis.com/auth/tasks"};
    std::string authUrl = "https://accounts.google.com/o/oauth2/auth";

    std::string token;
    std::string refreshToken;

   public:
    GoogleOauth(const std::string &credPath);
    void showCredential();
    void setCredential(const std::string &cred);
    std::string getAuthorizationUrl();
    std::string getAccessToken(const std::string &code);
    std::string getRefreshToken();
};

#endif