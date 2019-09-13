//
// Created by chris on 12.09.19.
//

#ifndef RDMA_LOGGING_HPP
#define RDMA_LOGGING_HPP

#endif //RDMA_LOGGING_HPP

#include "../config/Config.h"

#include <iostream>
#include <string>

namespace rdma {
    class Logging {
    public:
        static void debug(string filename, int line, string msg) {
            // avoid unused variable warning
            (void) filename;
            (void) line;
            (void) msg;
#ifdef DEBUG
            if (Config::LOGGING_LEVEL <= 1)
                log("[DEBUG]: ", filename, line, msg);
#endif
        }

        static void info(string msg) {
            if (Config::LOGGING_LEVEL <= 2) {
                log("[INFO]", msg);
            }
        }

        static void info(string filename, int line, string msg) {
            if (Config::LOGGING_LEVEL <= 2) {
                log("[INFO]", filename, line, msg);
            }
        }

        static void warn(string msg) {
            if (Config::LOGGING_LEVEL <= 3) {
                log("[WARNING]", msg);
            }
        }

        static void warn(string filename, int line, string msg) {
            if (Config::LOGGING_LEVEL <= 3) {
                log("[WARNING]", filename, line, msg);
            }
        }

        static void error(string filename, int line, string msg) {
            if (Config::LOGGING_LEVEL <= 4) {
                log("[ERROR]", filename, line, msg);
            }
        }

        static void errorNo(string filename, int line, char *msg, int errNo) {
            if (Config::LOGGING_LEVEL <= 4)
                logNo("[ERROR]", filename, line, errNo, msg);
        }

        static void fatal(string filename, int line, string msg) {
            if (Config::LOGGING_LEVEL <= 5)
                log("[FATAL]: ", filename, line, msg);
            exit(1);
        }


    private:
        static void log(string type, string msg) {
            cerr << type << msg << endl;
        }

        static void log(string type, string filename, int line, string msg) {
            cerr << type << filename << " at " << line << " " << msg << endl;
        }

        static void logNo(string type, string filename, int line, int errNo, char *errorMsg) {
            cerr << type << filename << " at " << line << " : " << errNo << " : " << errorMsg << endl;
        }
    };
}