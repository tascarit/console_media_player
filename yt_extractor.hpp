#include <string>

using namespace std;

#ifdef _WIN32
#define POPEN _popen
#define PCLOSE _pclose
#else
#define POPEN popen
#define PCLOSE pclose
#endif
std::string ExecuteCommand(const char* cmd) {
    std::array<char, 128> buffer;
    std::string result;
    std::unique_ptr<FILE, decltype(&PCLOSE)> pipe(POPEN(cmd, "r"), PCLOSE);
    if (!pipe) {
        throw std::runtime_error("popen() failed!");
    }
    while (fgets(buffer.data(), static_cast<int>(buffer.size()), pipe.get()) != nullptr) {
        result += buffer.data();
    }
    return result;
}

string GetStreamUrl(const string& youtube_url) {
    string cmd = "yt-dlp -g --no-warnings " + youtube_url;

    try {
        string url = ExecuteCommand(cmd.c_str());
        size_t pos = url.find('\n');

        if (pos != std::string::npos)
            url = url.substr(0, pos);

        return url;
    }
    catch (const exception& e) {
        throw runtime_error(string("Error getting stream URL: ") + e.what());
    }
}
