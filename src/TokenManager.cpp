#include "TokenManager.h"

#include <fstream>
#include <string>

void TokenManager::saveTokens(const std::string& token,
                              const std::string& refreshToken) {
    std::ofstream tokenFile(tokenFile, std::ios::binary);

    tokenFile << token << std::endl << refreshToken;
    tokenFile.close();
}

std::string TokenManager::getToken() {
    std::ifstream tokenFile(tokenFile, std::ios::binary);
    std::string token, refreshToken;

    tokenFile >> token >> refreshToken;
    tokenFile.close();

    return token;
}

std::string TokenManager::getRefreshToken() {
    std::ifstream tokenFile(tokenFile, std::ios::binary);
    std::string token, refreshToken;

    tokenFile >> token >> refreshToken;
    tokenFile.close();

    return refreshToken;
}
