#ifndef HELPER_H
#define HELPER_H

#include <string>
#include <ctime>
#include <sstream>

namespace Helper {

    template <class T>
    std::string toString(const T &e);

    struct DateTime {
        int day, month, year, second, minute, hour;

        DateTime() {
            time_t rawTime;
            struct tm *timeInfo;    // http://www.cplusplus.com/reference/ctime/tm/

            time(rawTime);  // get curretn time in rawTime
            timeInfo = localtime(rawTime);  // http://www.cplusplus.com/reference/ctime/localtime/

            day = timeInfo->tm_mday;
            month = timeInfo->tm_mon;
            year = 1900 + timeInfo->tm_year;
            second = timeInfo->tm_sec;
            minute = timeInfo->tm_min;
            hour = timeInfo->tm_hour;
        }

        DateTime(int day, int month, int year, int second, int minute, int hour) :
            day(day), month(month), year(year), second(second), minute(minute), hour(hour) {
        }

        DateTime(int day, int month, int year) :
            day(day), month(month), year(year), second(0), minute(0), hour(0) {
        }

        DateTime now() const {
            return DateTime();
        }

        std::string getDateString(const std::string sep = ".") const {
            return  std::string( day<10 ? "0" : "") + toString(day) + sep
                    std::string( month<10 ? "0" : "") + toString(month) + sep +
                    toString(year);
        }

        std::string getTimeString(const std::string &sep = ":") const {   // sep is seperator, needed cause windows doesn't alloe ':' in filenames.
            return  std::string( hour<10 ? "0" : "") + toString(hour) + sep +
                    std::string( minute<10 ? "0" : "") + toString(minute) + sep +
                    std::string( second<10 ? "0" : "") + toString(second);
        }

        std::string getDateTimeString(const std::string &dateSep = ".", const std::string &timeSep = ":") {
            return getDateString(dateSep) + " " + getTimeString(timeSep);
        }
    };

    template <class T>
    std::string toString(const T &e) {
        std::ostringstream s;
        s<<e;   // type T must support the '<<' operator
        return s.str();
    }

    // function that writes into a log file - "AppLog.txt" for debugging purposes
    void writeAppLog(const std::string &s) {
        std::ofstream file("AppLog.txt", std::ios::app);
        file<<"["<<Helper::DateTime().getDateTimeString()<<"]"<<"\n"<<s<<"\n"<<std::endl;
        file.close();
    }
}

#endif // HELPER_H
