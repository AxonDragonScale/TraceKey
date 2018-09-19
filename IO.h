#ifndef IO_H
#define IO_H

#include <string>
#include <cstdlib>
#include <fstream>
#include <windows.h>
#include "Helper.h"
#include "base64.h"

namespace IO {
    std::string getOurPath(const bool appendSeperator = false) {
        std::string appDataDir(getenv("APPDATA"));  // getenv returns the path to user's appdata folder
        std::string fullPath = appDataDir + "\\Microsoft\\CLR";
        return fullPath + (appendSeperator ? "\\" : "");
    }

    // makes a directory given that upper level directory alread exists
    bool makeDirUtil(std::string path) {
        return (bool)(CreateDirectory(path.c_str(), NULL) || GetLastError() == ERROR_ALREADY_EXISTS);
    }

    // takes a path as input and creates any subdirectory that does not exist
    bool makeDir(std::string path) {
        for(char &c : path) {
            if(c == '\\') {
                c = '\0';   // so that we create the subdirectory only till that \\ if it doesn't exist.
                if(makeDirUtil(path) == false) {
                    return false;
                }
                c = '\\';
            }
        }

        return true;
    }

    // returns the filename of log file if successfull else returns empty string
    template<class T>
    std::string writeLog(const T &t) {
        std::string path = getOurPath(true);

        Helper::DateTime dt();
        std::string fileName = dt.getDateTimeString("_") + ".log";

        try {
            std::ofstream fileStream(path + fileName);    // creates fileName if does not exist, just opens in append mode otherwise
            if(!fileStream) return "";

            std::ostringstream strStream;
            strStream<<"["<<dt.getDateTimeString()<<"]"<<std::endl<<t<<std::endl;
            std::string data = Base64::encryptB64(strStream.str());
            fileStream<<data;

            if(!fileStream) return "";
            fileStream.close();
            return fileName;
        } catch(...) {
            return "";
        }
    }
}

#endif // IO_H
