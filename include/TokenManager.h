#ifndef TOKENMANAGER_H
#define TOKENMANAGER_H

#include <string>

class TokenManager {
   private:
    std::string tokenFileName = "tokens.dat";

   public:
    void showToken();
    void saveTokens(const std::string &token, const std::string &refreshToken);
    std::string getToken();
    std::string getRefreshToken();
};

#endif  // TOKENMANAGER_H