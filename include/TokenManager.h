#ifndef TOKENMANAGER_H
#define TOKENMANAGER_H

#include <string>

class TokenManager {
   private:
    std::string tokenFileName = "tokens.dat";
    std::string clientId;
    std::string clientSecret;
    std::string token;
    std::string refreshToken;

    void readTokens();

   public:
    void showToken();
    void saveTokens(const std::string &clientId,
                    const std::string &clientSecret, const std::string &token,
                    const std::string &refreshToken);
    std::string getToken();
    std::string getRefreshToken();
    std::string getClientId();
    std::string getClientSecret();
};

#endif  // TOKENMANAGER_H