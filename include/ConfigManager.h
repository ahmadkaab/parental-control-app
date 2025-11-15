#ifndef CONFIGMANAGER_H
#define CONFIGMANAGER_H

#include <string>

class ConfigManager {
public:
    ConfigManager(const std::string& filename);
    std::string getBotToken() const;
    std::string getChatID() const;

private:
    std::string botToken;
    std::string chatID;
};

#endif // CONFIGMANAGER_H
