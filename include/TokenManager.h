#ifndef TOKENMANAGER_H
#define TOKENMANAGER_H

#include <string>

struct GoogleTokens {
    std::string clientId;
    std::string clientSecret;
    std::string token;
    std::string refreshToken;
};

class TokenManager {
   private:
    std::string tokenFileName = "tokens.dat";
    std::string clientId;
    std::string clientSecret;
    std::string token;
    std::string refreshToken;

    void readTokens();

   public:
    TokenManager();
    void showToken();
    void saveTokens(const std::string &clientId,
                    const std::string &clientSecret, const std::string &token,
                    const std::string &refreshToken);
    void saveTokens(GoogleTokens tokens);
    GoogleTokens getTokens();
    std::string getToken();
    std::string getRefreshToken();
    std::string getClientId();
    std::string getClientSecret();
};

#endif  // TOKENMANAGER_H