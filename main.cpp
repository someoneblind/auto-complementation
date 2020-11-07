#include <iostream>
#include <string>
#include <string.h>
#include <memory>
#include <stdio.h>
#include <termios.h>
#include <unistd.h>

#include "complementer.h"

#define DEVNAME_NAME    "devname"
#define UARTSPEED_NAME  "uartspeed"
#define INPUTTYPE_NAME  "inputtype"

using namespace std;

set<string> getDevnames()
{
    set<string> uartdevs;

    using FilePtr = unique_ptr<FILE, decltype(&::pclose)>;
    string cmd("/bin/ls ");
    cmd.append("/dev/tty*");

    FilePtr ls(::popen(cmd.c_str(), "r"), ::pclose);
    if (ls == nullptr)
        return set<string>();

    vector<char> buf(1024);
    string ret;
    while (::fgets(buf.data(), buf.size(), ls.get())) {
        ret.append(buf.data());
    }

    string delimiter = "\n";
    size_t pos = 0;
    string token;
    while ((pos = ret.find(delimiter)) != std::string::npos) {
        token = ret.substr(0, pos);
        ret.erase(0, pos + delimiter.length());
        uartdevs.insert(token);
    }

    return uartdevs;
}

int main()
{
    set<string> voidSet = {} ;
    set<string> commands = { "set", "get", "send", "quit" };
    set<string> params = { "devname", "uartspeed", "inputtype" };
    set<string> devnames = getDevnames();
    set<string> uartspeeds = { "9600", "19200", "57600", "115200" };
    set<string> inputtypes = { "ascii", "hex", "dec" };

    struct termios oldt, newt;

    Complementer complementer;
    string str;
    char symbol;

    tcgetattr(STDIN_FILENO, &oldt);
    newt = oldt;
    newt.c_lflag &= ~(ICANON | ECHO);
    tcsetattr(STDIN_FILENO, TCSANOW, &newt);

    while (1) {
        symbol = static_cast<char>(getchar());

        if ((symbol == '\t') && (str[0] == '\\')) {

            if (count(str.begin(), str.end(), ' ') == 0)
                complementer.setSet(commands);
            else if ((count(str.begin(), str.end(), ' ') == 1)
                     && ((str.substr(1, 3) == "set")
                         || (str.substr(1, 3) == "get")))
                complementer.setSet(params);
            else if ((count(str.begin(), str.end(), ' ') == 2)
                     && (str.substr(str.find_first_of(" ") + 1, strlen(DEVNAME_NAME)) == DEVNAME_NAME))
                complementer.setSet(devnames);
            else if ((count(str.begin(), str.end(), ' ') == 2)
                     && (str.substr(str.find_first_of(" ") + 1, strlen(UARTSPEED_NAME)) == UARTSPEED_NAME))
                complementer.setSet(uartspeeds);
            else if ((count(str.begin(), str.end(), ' ') == 2)
                     && (str.substr(str.find_first_of(" ") + 1, strlen(INPUTTYPE_NAME)) == INPUTTYPE_NAME))
                complementer.setSet(inputtypes);
            else
                complementer.setSet(voidSet);

            cout  << endl;
            complementer.setInput(str.substr(str.find_last_of("\\ ") + 1));

            if (complementer.getHints().size() > 0) {
                for (const string& hint : complementer.getHints())
                    cout << hint << "\t";
                cout  << endl;
                if (str[str.size() - 1] != ' ')
                    str += complementer.getHint(str.substr(1)).substr(str.size() - str.find_last_of("\\ ") - 1);
            }

            if (complementer.getHints().size() == 1) {
                str += ' ';
                cout << str;
            }
            else {
                cout << str;
            }

        }
        else if (symbol == '\n') {

            if (!str.find("\\quit")) {
                str.clear();
                cout << static_cast<char>(symbol);
                break;
            }
            else {
                str.clear();
                cout << static_cast<char>(symbol);
            }

        }
        else if ((symbol == '\b') || (symbol == 127)) {
            if (str.length() > 0) {
                str.pop_back();
                cout << '\b' << ' ' << '\b';
            }
        }
        else if (symbol >= 0x20) {
            str += symbol;
            cout << static_cast<char>(symbol);
        }
    }

    tcsetattr(STDIN_FILENO, TCSANOW, &oldt);

    return 0;
}
