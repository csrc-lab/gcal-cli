#ifndef TOKENMANAGER_H
#define TOKENMANAGER_H

#include <string>

class TokenManager {
   private:
    std::string tokenFile = "tokens.dat";

   public:
    void showToken();
    void saveToken(const std::string &token, const std::string &refreshToken);
    std::string getToken();
    std::string getRefreshToken();
};

#endif  // TOKENMANAGER_H