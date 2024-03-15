#ifndef CONFIGMANAGER_H
#define CONFIGMANAGER_H

#include <string>

class ConfigManager {
public:
    static void showConfiguration();
    static void setConfiguration(const std::string &config);
};

#endif // CONFIGMANAGER_H
