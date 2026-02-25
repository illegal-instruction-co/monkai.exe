cmake_minimum_required(VERSION 3.20)
project(monkai LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include(FetchContent)

FetchContent_Declare(
    nlohmann_json
    GIT_REPOSITORY https://github.com/nlohmann/json.git
    GIT_TAG        v3.11.3
)

set(BUILD_TESTING OFF CACHE BOOL "" FORCE)
set(BUILD_SHARED_LIBS OFF CACHE BOOL "" FORCE)
set(BUILD_CURL_EXE OFF CACHE BOOL "" FORCE)
set(CURL_USE_SCHANNEL ON CACHE BOOL "" FORCE)
set(CURL_USE_OPENSSL OFF CACHE BOOL "" FORCE)
set(CURL_ZLIB OFF CACHE BOOL "" FORCE)
set(HTTP_ONLY ON CACHE BOOL "" FORCE)
set(CURL_DISABLE_LDAP ON CACHE BOOL "" FORCE)
set(CURL_DISABLE_LDAPS ON CACHE BOOL "" FORCE)
set(CURL_DISABLE_TELNET ON CACHE BOOL "" FORCE)
set(CURL_DISABLE_DICT ON CACHE BOOL "" FORCE)
set(CURL_DISABLE_FILE ON CACHE BOOL "" FORCE)
set(CURL_DISABLE_TFTP ON CACHE BOOL "" FORCE)
set(CURL_DISABLE_RTSP ON CACHE BOOL "" FORCE)
set(CURL_DISABLE_POP3 ON CACHE BOOL "" FORCE)
set(CURL_DISABLE_IMAP ON CACHE BOOL "" FORCE)
set(CURL_DISABLE_SMTP ON CACHE BOOL "" FORCE)
set(CURL_DISABLE_GOPHER ON CACHE BOOL "" FORCE)
set(CURL_DISABLE_MQTT ON CACHE BOOL "" FORCE)

FetchContent_Declare(
    curl
    GIT_REPOSITORY https://github.com/curl/curl.git
    GIT_TAG        curl-8_5_0
)

FetchContent_MakeAvailable(nlohmann_json curl)

add_executable(monkai
    src/main.cpp
    src/version.cpp
    src/OpenAIClient.cpp
    src/Memento.cpp
    src/ToolEngine.cpp
)

target_include_directories(monkai PRIVATE src)
target_link_libraries(monkai PRIVATE
    nlohmann_json::nlohmann_json
    libcurl_static
)

target_compile_definitions(monkai PRIVATE CURL_STATICLIB)

if(WIN32)
    target_link_libraries(monkai PRIVATE ws2_32 crypt32)
endif()
