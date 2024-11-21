#include "roblox.h"

namespace fs = std::filesystem;
std::string RBXPath = std::getenv(XorStr("LOCALAPPDATA")) + std::string(XorStr("\\Roblox\\logs"));

const std::regex RENDER_VIEW_PATTERN(XorStr(R"(\[FLog::SurfaceController\] SurfaceController\[_:\d\]::initialize view\([A-F0-9]{16}\))"));

std::unique_ptr<std::ifstream> GetLog() {
    std::string fileName;
    for (const auto& entry : std::filesystem::directory_iterator(RBXPath)) {
        if (entry.is_regular_file() && entry.path().filename().string().find(XorStr("_Player_")) != std::string::npos) {
            fileName = entry.path().string();
            break;
        }
    }

    if (!fileName.empty()) {
        return std::make_unique<std::ifstream>(fileName);
    }

    return nullptr;
}

long long GetRenderViewFromLog() {
    auto logFilePtr = GetLog();
    if (logFilePtr && logFilePtr->is_open()) {
        std::stringstream buffer;
        buffer << logFilePtr->rdbuf();
        std::string logContent = buffer.str();

        std::vector<std::string> renderViews;
        auto it = std::sregex_iterator(logContent.begin(), logContent.end(), RENDER_VIEW_PATTERN);
        auto end = std::sregex_iterator();

        while (it != end) {
            renderViews.push_back(it->str());
            ++it;
        }

        if (!renderViews.empty()) {
            std::string matchedStr = renderViews.back();
            std::regex addressPattern(XorStr(R"([A-F0-9]{16})"));
            std::smatch addressMatch;

            if (std::regex_search(matchedStr, addressMatch, addressPattern)) {
                return std::stoll(addressMatch[0].str(), nullptr, 16);
            }
        }
    }
    return 0;
}

void ClearLog() {
    if (fs::exists(RBXPath)) {
        for (const auto& entry : fs::directory_iterator(RBXPath)) {
            try {
                if (entry.is_regular_file()) {
                    fs::remove(entry.path());
                }
                else if (entry.is_directory()) {
                    fs::remove_all(entry.path());
                }
            }
            catch (const std::exception& e) {
                //std::cerr << "Error deleting file: " << e.what() << std::endl;
            }
        }
    }
}