#include "GoogleOauth.h"

#include <cpr/cpr.h>

#include <fstream>
#include <iostream>
#include <nlohmann/json.hpp>
#include <sstream>
#include <vector>

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

std::string GoogleOauth::getAccessToken(const std::string &code) {
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
        // Parse the JSON response
        auto jsonResponse = nlohmann::json::parse(r.text);
        // jsonResponse
        std::cout << "Access Token: " << jsonResponse["access_token"]
                  << std::endl;
        this->token = jsonResponse["access_token"];
        this->refreshToken = jsonResponse["refresh_token"];
    } else {
        std::cerr << "Failed to exchange authorization code: " << r.status_code
                  << std::endl;
    }

    return this->token;
}

std::string GoogleOauth::getRefreshToken() { return this->refreshToken; }