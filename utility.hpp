#include <utility>
#include "defs.hpp"

#ifdef _WIN32
#include <windows.h>
#else
#include <sys/ioctl.h>
#include <unistd.h>
#endif

std::pair<int, int> GetTerminalSize() {
    int cols = 80;
    int rows = 24;

#ifdef _WIN32
    CONSOLE_SCREEN_BUFFER_INFO csbi;
    HANDLE hStdOut = GetStdHandle(STD_OUTPUT_HANDLE);

    if (GetConsoleScreenBufferInfo(hStdOut, &csbi)) {
        cols = csbi.srWindow.Right - csbi.srWindow.Left + 1;
        rows = csbi.srWindow.Bottom - csbi.srWindow.Top + 1;
    }
#else
    struct winsize size;
    if (ioctl(STDOUT_FILENO, TIOCGWINSZ, &size) == 0) {
        cols = size.ws_col;
        rows = size.ws_row;
    }
#endif

    return { cols, rows };
}

std::string GenerateLabel() {
    int r = 245, g = 204, b = 159;

    std::string ret = "";

    for (int i = 0; i < 5; i++) {
        ret += '|';
        if (i != 2) {
            for (int j = 0; j < 92; j++) {
                ret += "\033[38;2;" + to_string(min(r, 255)) + ';' + to_string(min(g, 255)) + ';' + to_string(min(b, 255)) + 'm' + '=' + "\033[0m";
                (g <= 159) ? r++ : ((r <= 159) ? ((b >= 245) ? g-- : b++) : r--);
            }
        }
        else {
            for (int k = 0; k < 35; k++) {
                ret += "\033[38;2;155;255;143m=\033[0m";
            }
            ret += "[Console Media Player]";
            for (int k = 0; k < 35; k++) {
                ret += "\033[38;2;155;255;143m=\033[0m";
            }
            (g <= 159) ? r++ : ((r <= 159) ? ((b >= 245) ? g-- : b++) : r--);
        }
        ret += "|\n";
    }

    ret += '\n';

    return ret;
}

void HelpCommand() {
    std::string label = GenerateLabel();
    std::cerr << label << helpCommand << std::endl;
}