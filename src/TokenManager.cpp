#include "TokenManager.h"

#include <cstdlib>
#include <fstream>
#include <iostream>
#include <string>

std::string getHomeDirectory() {
    std::string homeDir;
#ifdef _WIN32
    // Windows
    char* userProfile = getenv("USERPROFILE");
    if (userProfile) homeDir = userProfile;
#else
    // Unix/Linux/macOS
    char* home = getenv("HOME");
    if (home) homeDir = home;
#endif
    return homeDir;
}

TokenManager::TokenManager() {
    tokenFileName = getHomeDirectory() + "/.gcal_cli_tokens";
    std::cout << "Token file name: " << tokenFileName << std::endl;
}

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

void TokenManager::saveTokens(GoogleTokens tokens) {
    saveTokens(tokens.clientId, tokens.clientSecret, tokens.token,
               tokens.refreshToken);
}

void TokenManager::readTokens() {
    std::ifstream tokenFile(tokenFileName, std::ios::binary);
    tokenFile >> clientId >> clientSecret >> token >> refreshToken;
    tokenFile.close();
}

GoogleTokens TokenManager::getTokens() {
    if (token.empty()) {
        readTokens();
    }
    return {clientId, clientSecret, token, refreshToken};
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
