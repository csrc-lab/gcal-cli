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

    std::string access_token = oauth.getAccessToken(code);
    std::cout << "Access Token: " << access_token << std::endl;

    std::string refresh_token = oauth.getRefreshToken();
    std::cout << "Refresh Token: " << refresh_token << std::endl;

    std::string client_id = oauth.getClientId();
    std::string client_secret = oauth.getClientSecret();

    TokenManager tokenManager;
    tokenManager.saveTokens(client_id, client_secret, access_token,
                            refresh_token);

    std::cout << "Configuration set successfully" << std::endl;
}
