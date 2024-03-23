#include "TokenManager.h"

#include <fstream>
#include <string>

void TokenManager::saveTokens(const std::string& clientId,
                              const std::string& clientSecret,
                              const std::string& token,
                              const std::string& refreshToken) {
    std::ofstream tokenFile(tokenFileName, std::ios::binary);

    tokenFile << clientId << std::endl;
    tokenFile << clientSecret << std::endl;
    tokenFile << token << std::endl;
    tokenFile << refreshToken << std::endl;
    tokenFile.close();
}

void TokenManager::readTokens() {
    std::ifstream tokenFile(tokenFileName, std::ios::binary);
    tokenFile >> clientId >> clientSecret >> token >> refreshToken;
    tokenFile.close();
}

std::string TokenManager::getToken() {
    if (token.empty()) {
        readTokens();
    }
    return token;
}

std::string TokenManager::getRefreshToken() {
    if (refreshToken.empty()) {
        readTokens();
    }
    return refreshToken;
}

std::string TokenManager::getClientId() {
    if (clientId.empty()) {
        readTokens();
    }
    return clientId;
}

std::string TokenManager::getClientSecret() {
    if (clientSecret.empty()) {
        readTokens();
    }
    return clientSecret;
}
