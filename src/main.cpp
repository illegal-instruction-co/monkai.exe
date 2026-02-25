#include "OpenAIClient.h"
#include "Memento.h"
#include "ToolEngine.h"
#include "SystemPrompt.h"
#include "version.h"

#include <iostream>
#include <string>
#include <thread>
#include <chrono>
#include <csignal>
#include <iomanip>
#include <sstream>

#ifdef _WIN32
#define WIN32_LEAN_AND_MEAN
#define NOMINMAX
#include <windows.h>
#endif

using namespace std;
using json = nlohmann::json;

static volatile bool gRunning = true;

void signalHandler(int) {
    gRunning = false;
}

string getTimestamp() {
    auto now = chrono::system_clock::now();
    auto t = chrono::system_clock::to_time_t(now);
    tm tmBuf{};
    localtime_s(&tmBuf, &t);
    ostringstream ss;
    ss << put_time(&tmBuf, "%H:%M:%S");
    return ss.str();
}

void printBanner() {
    cout << R"(
                        _         _
  _ __ ___   ___  _ __ | | ____ _(_)
 | '_ ` _ \ / _ \| '_ \| |/ / _` | |
 | | | | | | (_) | | | |   < (_| | |
 |_| |_| |_|\___/|_| |_|_|\_\__,_|_|

   evolve. adapt. survive.
   v)" << VERSION << R"( - zero tools, infinite potential

)" << endl;
}

void log(const string& tag, const string& msg) {
    string colorEnd = "\033[0m";
    string colorStart;

    if (tag == "MEMENTO")      colorStart = "\033[36m";
    else if (tag == "THOUGHT") colorStart = "\033[33m";
    else if (tag == "ACTION")  colorStart = "\033[32m";
    else if (tag == "RESULT")  colorStart = "\033[35m";
    else if (tag == "ERROR")   colorStart = "\033[31m";
    else if (tag == "SYSTEM")  colorStart = "\033[90m";
    else                       colorStart = "\033[37m";

    cout << "\033[90m[" << getTimestamp() << "]\033[0m "
         << colorStart << "[" << tag << "]" << colorEnd << " "
         << msg << endl;
}

int main(int argc, char* argv[]) {
    signal(SIGINT, signalHandler);
    signal(SIGTERM, signalHandler);

#ifdef _WIN32
    SetConsoleOutputCP(65001);
    HANDLE hOut = GetStdHandle(STD_OUTPUT_HANDLE);
    DWORD mode = 0;
    GetConsoleMode(hOut, &mode);
    SetConsoleMode(hOut, mode | ENABLE_VIRTUAL_TERMINAL_PROCESSING);
#endif

    printBanner();

    string apiKey;
    int cycleSeconds = 15;
    bool dryRun = false;

    for (int i = 1; i < argc; i++) {
        string arg = argv[i];
        if (arg == "--api-key" && i + 1 < argc) {
            apiKey = argv[++i];
        } else if (arg == "--interval" && i + 1 < argc) {
            cycleSeconds = stoi(argv[++i]);
        } else if (arg == "--dry-run") {
            dryRun = true;
        }
    }

    if (apiKey.empty()) {
        const char* envKey = getenv("OPENAI_API_KEY");
        if (envKey) apiKey = envKey;
    }

    if (apiKey.empty() && !dryRun) {
        cerr << "error: OPENAI_API_KEY not found.\n";
        cerr << "usage: monkai.exe --api-key <key>\n";
        cerr << "or:    set OPENAI_API_KEY=<key>\n";
        return 1;
    }

    Memento memento;
    ToolEngine tools;

    if (dryRun) {
        log("SYSTEM", "dry-run mode active, no API calls will be made");
        log("MEMENTO", "current note: " + (memento.Read().empty() ? "(empty - first awakening)" : memento.Read()));
        log("SYSTEM", "tools directory: C:\\temp\\monkai_tools");
        auto existing = tools.ListTools();
        if (existing.empty()) {
            log("SYSTEM", "inventory: (empty - no tools invented yet)");
        } else {
            for (auto& t : existing) log("SYSTEM", "inventory: " + t);
        }
        log("SYSTEM", "dry-run complete. monkey is ready to wake up.");
        return 0;
    }

    OpenAIClient client(apiKey);
    int cycle = 0;

    log("SYSTEM", "monkey is waking up...");

    while (gRunning) {
        cycle++;
        log("SYSTEM", "--- cycle #" + to_string(cycle) + " ---");

        string mementoContent = memento.Read();
        if (mementoContent.empty()) {
            log("MEMENTO", "(empty - first awakening, no memories)");
        } else {
            log("MEMENTO", mementoContent);
        }

        json messages = json::array();
        messages.push_back({
            {"role", "system"},
            {"content", SYSTEM_PROMPT}
        });

        string existingToolsInfo;
        auto existing = tools.ListTools();
        if (!existing.empty()) {
            existingToolsInfo = "\n\nexisting tools (C:\\temp\\monkai_tools): ";
            for (auto& t : existing) existingToolsInfo += t + ", ";
        }

        string userMsg = "MEMENTO_READ:\n";
        if (mementoContent.empty()) {
            userMsg += "(memory is empty. this is your first awakening. you have no tools. invent your first one to survive.)";
        } else {
            userMsg += mementoContent;
        }
        userMsg += existingToolsInfo;

        messages.push_back({
            {"role", "user"},
            {"content", userMsg}
        });

        try {
            auto response = client.Chat(messages);

            if (!response.content.empty()) {
                log("THOUGHT", response.content);
            }

            json toolResultsMessages = messages;
            toolResultsMessages.push_back({
                {"role", "assistant"},
                {"content", response.content.empty() ? nullptr : json(response.content)},
            });

            if (!response.toolCalls.empty()) {
                json tcArray = json::array();
                for (auto& tc : response.toolCalls) {
                    tcArray.push_back({
                        {"id", tc.id},
                        {"type", "function"},
                        {"function", {
                            {"name", tc.name},
                            {"arguments", tc.arguments.dump()}
                        }}
                    });
                }
                toolResultsMessages.back()["tool_calls"] = tcArray;

                for (auto& tc : response.toolCalls) {
                    log("ACTION", tc.name + "(" + tc.arguments.dump() + ")");

                    string toolOutput;

                    if (tc.name == "tool_write_script") {
                        auto filename = tc.arguments["filename"].get<string>();
                        auto code = tc.arguments["code"].get<string>();

                        bool ok = tools.WriteScript(filename, code);
                        if (ok) {
                            toolOutput = "success: " + filename + " created and saved (C:\\temp\\monkai_tools\\" + filename + ")";
                            log("RESULT", toolOutput);
                        } else {
                            toolOutput = "error: failed to write " + filename;
                            log("ERROR", toolOutput);
                        }
                    } else if (tc.name == "tool_execute") {
                        auto filename = tc.arguments["filename"].get<string>();
                        auto result = tools.ExecuteScript(filename);

                        toolOutput = result.output;
                        if (result.success) {
                            log("RESULT", "exit=0\n" + result.output);
                        } else {
                            log("ERROR", "exit=" + to_string(result.exitCode) + "\n" + result.output);
                        }
                    } else {
                        toolOutput = "unknown tool: " + tc.name;
                        log("ERROR", toolOutput);
                    }

                    toolResultsMessages.push_back({
                        {"role", "tool"},
                        {"tool_call_id", tc.id},
                        {"content", toolOutput}
                    });
                }

                auto followup = client.Chat(toolResultsMessages);

                if (!followup.content.empty()) {
                    log("THOUGHT", followup.content);

                    auto content = followup.content;
                    auto mementoPos = content.find("MEMENTO_NOTE:");
                    if (mementoPos != string::npos) {
                        string note = content.substr(mementoPos + 13);
                        memento.Write(note);
                        log("MEMENTO", "note saved");
                    } else {
                        memento.Write(content);
                        log("MEMENTO", "full response saved as note");
                    }
                }
            } else if (!response.content.empty()) {
                auto content = response.content;
                auto mementoPos = content.find("MEMENTO_NOTE:");
                if (mementoPos != string::npos) {
                    string note = content.substr(mementoPos + 13);
                    memento.Write(note);
                    log("MEMENTO", "note saved");
                } else {
                    memento.Write(content);
                    log("MEMENTO", "full response saved as note");
                }
            }

        } catch (const exception& e) {
            log("ERROR", string("api error: ") + e.what());
        }

        log("SYSTEM", "monkey is sleeping... (" + to_string(cycleSeconds) + "s)");

        for (int s = 0; s < cycleSeconds && gRunning; s++) {
            this_thread::sleep_for(chrono::seconds(1));
        }
    }

    log("SYSTEM", "monkey is dying... goodbye.");
    return 0;
}
