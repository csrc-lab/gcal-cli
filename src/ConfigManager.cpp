#include "ConfigManager.h"

#include <fstream>
#include <iostream>

#include "GoogleOauth.h"
#include "TokenManager.h"

bool fileExists(const std::string& name) {
    std::ifstream f(name.c_str());
    return f.good();
}

void ConfigManager::showConfiguration() {
    // Implement the logic to show configuration
    std::cout << "Show the current configuration" << std::endl;
    TokenManager tokenManager;
    std::string token = tokenManager.getToken();
    std::string refreshToken = tokenManager.getRefreshToken();

    std::cout << "Access Token: " << token << std::endl;
    std::cout << "Refresh Token: " << refreshToken << std::endl;
}

void ConfigManager::setConfiguration(const std::string& credPath) {
    std::string localCredPath = credPath;
    if (localCredPath.empty()) {
        localCredPath = "credential.json";
    }

    if (!fileExists(localCredPath)) {
        std::cerr << "Error: Credential file not found" << std::endl;
        return;
    }
    GoogleOauth oauth(localCredPath);
    oauth.showCredential();

    std::string authorization_url = oauth.getAuthorizationUrl();

    std::cout << "\033[1;31mPlease copy the following URL and paste it into "
                 "your browser "
                 "to proceed with Google login:\033[0m\n"
              << authorization_url << std::endl;

    std::string code;
    std::cout << "\033[1;31mEnter the code: \033[0m";
    std::cin >> code;

    try {
        GoogleTokens tokens = oauth.getAccessToken(code);
        std::cout << "Access Token: " << tokens.token << std::endl;
        std::cout << "Refresh Token: " << tokens.refreshToken << std::endl;

        TokenManager tokenManager;
        tokenManager.saveTokens(tokens);

        std::cout << "Configuration set successfully" << std::endl;
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return;
    }
}

void ConfigManager::refreshConfiguration() {
    TokenManager tokenManager;
    GoogleTokens tokens = tokenManager.getTokens();
    GoogleOauth oauth(tokens);
    try {
        GoogleTokens newTokens = oauth.refreshTokens();
        // std::cout << "Access Token: " << newTokens.token << std::endl;
        // std::cout << "Refresh Token: " << newTokens.refreshToken <<
        // std::endl;

        tokenManager.saveTokens(newTokens);

        std::cout << "Configuration refreshed successfully" << std::endl;
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return;
    }
}
