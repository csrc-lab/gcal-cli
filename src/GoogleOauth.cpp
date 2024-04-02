#include "GoogleOauth.h"

#include <cpr/cpr.h>

#include <fstream>
#include <iostream>
#include <nlohmann/json.hpp>
#include <sstream>
#include <vector>

#include "ProfileManager.h"

GoogleOauth::GoogleOauth(const std::string &credPath) {
    // Implement the logic to read the credential from the file
    std::cout << "Read the credential from the file: " << credPath << std::endl;

    // read credential from  json file
    nlohmann::json j;
    std::ifstream i(credPath);
    i >> j;

    try {
        this->clientSecret =
            j.at("installed").at("client_secret").get<std::string>();
        this->clientId = j.at("installed").at("client_id").get<std::string>();
    } catch (nlohmann::json::out_of_range &e) {
        std::cerr << "Error: " << e.what() << std::endl;
    }
}

GoogleOauth::GoogleOauth(GoogleTokens &tokens) {
    this->clientId = tokens.clientId;
    this->clientSecret = tokens.clientSecret;
    this->token = tokens.token;
    this->refreshToken = tokens.refreshToken;
}

void GoogleOauth::showCredential() {
    std::cout << "Show the current credential" << std::endl;

    std::cout << "Client ID: " << this->clientId << std::endl;
    std::cout << "Client Secret: " << this->clientSecret << std::endl;
}

std::string GoogleOauth::getAuthorizationUrl() {
    // Implement the logic to get the authorization URL
    std::stringstream scopes_ss;
    for (auto it = scopes.begin(); it != scopes.end(); ++it) {
        if (it != scopes.begin()) {
            scopes_ss << "%20";  // URL encoded space
        }
        scopes_ss << *it;
    }

    return authUrl + "?response_type=code&client_id=" + clientId +
           "&redirect_uri=" + redirectUri + "&scope=" + scopes_ss.str() +
           "&access_type=offline&prompt=consent";
}

GoogleTokens GoogleOauth::getAccessToken(const std::string &code) {
    std::cout << "Get the access token using the code: " << code << std::endl;

    const std::string tokenEndpoint = "https://oauth2.googleapis.com/token";

    cpr::Response r =
        cpr::Post(cpr::Url{tokenEndpoint},
                  cpr::Payload{{"code", code},
                               {"client_id", clientId},
                               {"client_secret", clientSecret},
                               {"redirect_uri", redirectUri},
                               {"grant_type", "authorization_code"}});

    if (r.status_code == cpr::status::HTTP_OK) {
        auto jsonResponse = nlohmann::json::parse(r.text);
        this->token = jsonResponse["access_token"];
        this->refreshToken = jsonResponse["refresh_token"];
    } else {
        std::cerr << "Failed to exchange authorization code: " << r.status_code
                  << std::endl;
        throw std::runtime_error("Failed to exchange authorization code");
    }

    return {clientId, clientSecret, token, refreshToken};
}

std::string GoogleOauth::getRefreshToken() { return this->refreshToken; }

std::string GoogleOauth::getClientId() { return this->clientId; }

std::string GoogleOauth::getClientSecret() { return this->clientSecret; }

GoogleTokens GoogleOauth::refreshTokens() {
    std::cout << "Refreshing the access token using the refresh token."
              << std::endl;

    if (this->refreshToken.empty()) {
        std::cerr << "Error: Refresh token is empty" << std::endl;
        throw std::runtime_error("Refresh token is empty");
    }

    const std::string tokenEndpoint = "https://oauth2.googleapis.com/token";

    // Make a POST request to the Google OAuth 2.0 token endpoint
    cpr::Response r =
        cpr::Post(cpr::Url{tokenEndpoint},
                  cpr::Payload{{"client_id", clientId},
                               {"client_secret", clientSecret},
                               {"refresh_token", this->refreshToken},
                               {"grant_type", "refresh_token"}});

    if (r.status_code == cpr::status::HTTP_OK) {
        // Parse the JSON response
        auto jsonResponse = nlohmann::json::parse(r.text);

        // Extract the new access token and optionally the new refresh token
        std::cout << "New Access Token: " << jsonResponse["access_token"]
                  << std::endl;
        this->token = jsonResponse["access_token"];

        // Some responses may include a new refresh token
        if (jsonResponse.contains("refresh_token")) {
            this->refreshToken = jsonResponse["refresh_token"];
        }
    } else {
        std::cerr << "Failed to refresh the access token: " << r.status_code
                  << std::endl;
        throw std::runtime_error("Failed to refresh the access token");
    }

    return {clientId, clientSecret, token, refreshToken};
}