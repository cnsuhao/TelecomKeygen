#include <iostream>
#include <algorithm>
#include <ctime>
#include <string>
#include "md5.h"

#if WIN32
#include <Windows.h>
#endif

class Utils {
public:
    std::string md5(std::string str, bool uppercase = true) {
        auto bytes = (unsigned char *) ((void *) (str.data()));
        _md5.Update(bytes, str.length());
        unsigned char result[16];
        _md5.Final(result);
        std::string md5str;
        for (auto bt : result) {
            char ch[3];
            sprintf(ch, "%02x", bt);
            md5str.append(ch);
        }
        if (uppercase) {
            std::transform(md5str.begin(), md5str.end(), md5str.begin(), toupper);
        }
        return md5str;
    }

private:
    CryptoPP::MD5 _md5;
};

class Keygen {
public:
    Keygen(std::string un, std::string pwd)
        :_un(un), _pwd(pwd) {

    }

    std::string gen() {
        Utils util;
        auto token = util.md5(getHMS());
        auto salt = token = token.substr(0, 8);
        token = "jepyid" + _un + token + _pwd;
        token = util.md5(token);
        token = token.substr(0, 20);
        token = "~ghca" + salt + "2007" + token + _un;
        return token;
    }

protected:
    std::string getHMS() {
        time_t timestamp = time(0);
        tm* tm = localtime(&timestamp);
        char buf[30];
        sprintf(buf, "%d%d%d", tm->tm_hour, tm->tm_min, tm->tm_sec);
        return std::string(buf);
    }

private:
    std::string _un;
    std::string _pwd;
};

int main(int argc, char* argv[]) {
    if (argc != 3) {
        std::cerr << "Usage:\n"
            << "\tTelecomKeygen username password\n"
            << "example\n\t$ ./TelecomKeygen CD028xxx@96301 12345678\n"
            ;
        exit(0);
    }
    Keygen keygen(argv[1], argv[2]);
    std::string key = keygen.gen();

    std::cout << key << std::endl;

#if WIN32 //for windows,copy result to clipboard
    if (OpenClipboard(NULL)) {
        HANDLE hMen;
        char *pMen;
        hMen = GlobalAlloc(GHND | GMEM_DDESHARE, key.length() + 1);
        if (hMen) {
            pMen = (char*) GlobalLock(hMen);
            strcpy(pMen, key.c_str());
            GlobalUnlock(hMen);
            EmptyClipboard();
            SetClipboardData(CF_TEXT, hMen);

            std::cout << "the result now copied to your clipboard.";
        }
    }
#endif

    return 0;
}
