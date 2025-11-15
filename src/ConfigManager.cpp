#include "ConfigManager.h"
#include <fstream>
#include <iostream> // For logging
#include <algorithm> // For std::find_if

// Function to trim leading and trailing whitespace
std::string trim(const std::string& str) {
    const std::string whitespace = " \t";
    const auto strBegin = str.find_first_not_of(whitespace);
    if (strBegin == std::string::npos)
        return ""; // no content

    const auto strEnd = str.find_last_not_of(whitespace);
    const auto strRange = strEnd - strBegin + 1;

    return str.substr(strBegin, strRange);
}

ConfigManager::ConfigManager(const std::string& filename) {
    std::ofstream logfile("startup_log.txt", std::ios::app);
    if (logfile.is_open()) {
        logfile << "ConfigManager created with filename: " << filename << std::endl;
    }

    std::ifstream configFile(filename);
    if (!configFile.is_open()) {
        if (logfile.is_open()) {
            logfile << "Failed to open config file." << std::endl;
            logfile.close();
        }
        // In a real application, you'd want better error handling here.
        // For now, we'll just leave the members empty.
        return;
    }

    if (logfile.is_open()) {
        logfile << "Config file opened successfully." << std::endl;
    }

    std::string line;
    while (std::getline(configFile, line)) {
        if (logfile.is_open()) {
            logfile << "Read line: " << line << std::endl;
        }
        size_t delimiterPos = line.find("=");
        if (delimiterPos != std::string::npos) {
            std::string key = trim(line.substr(0, delimiterPos));
            std::string value = trim(line.substr(delimiterPos + 1));
            if (logfile.is_open()) {
                logfile << "Parsed key: " << key << ", value: " << value << std::endl;
            }
            if (key == "BotToken") {
                botToken = value;
            } else if (key == "ChatID") {
                chatID = value;
            }
        }
    }

    if (logfile.is_open()) {
        logfile.close();
    }
}

std::string ConfigManager::getBotToken() const {
    return botToken;
}

std::string ConfigManager::getChatID() const {
    return chatID;
}
