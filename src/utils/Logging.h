//
// Created by chris on 12.09.19.
//

#ifndef RDMA_LOGGING_HPP
#define RDMA_LOGGING_HPP

#include "../config/Config.h"

#include <iostream>
#include <string>
#include <errno.h>

namespace rdma {

    class Logging {
    public:
        static void debug(std::string filename, int line, std::string msg) {
            // avoid unused variable warning
            (void) filename;
            (void) line;
            (void) msg;
#ifdef DEBUG
            if (Config::LOGGING_LEVEL <= 1)
                log("[DEBUG]: ", filename, line, msg);
#endif
        }

        static void info(std::string msg) {
            if (Config::LOGGING_LEVEL <= 2) {
                log("[INFO]", msg);
            }
        }

        static void info(std::string filename, int line, std::string msg) {
            if (Config::LOGGING_LEVEL <= 2) {
                log("[INFO]", filename, line, msg);
            }
        }

        static void warn(std::string msg) {
            if (Config::LOGGING_LEVEL <= 3) {
                log("[WARNING]", msg);
            }
        }

        static void warn(std::string filename, int line, std::string msg) {
            if (Config::LOGGING_LEVEL <= 3) {
                log("[WARNING]", filename, line, msg);
            }
        }

        static void error(std::string filename, int line, std::string msg) {
            if (Config::LOGGING_LEVEL <= 4) {
                log("[ERROR]", filename, line, msg);
            }
        }

        static void errorNo(std::string filename, int line, char *msg, int errNo) {
            if (Config::LOGGING_LEVEL <= 4)
                logNo("[ERROR]", filename, line, errNo, msg);
        }

        static void fatal(std::string filename, int line, std::string msg) {
            if (Config::LOGGING_LEVEL <= 5)
                log("[FATAL]: ", filename, line, msg);
            exit(1);
        }


    private:
        static void log(std::string type, std::string msg) {
            cerr << type << ' ' << msg << endl;
        }

        static void log(std::string type, std::string filename, int line, std::string msg) {
            cerr << type << ' ' << filename << " @ " << line << ":" << msg << endl;
        }

        static void logNo(std::string type, std::string filename, int line, int errNo, char *errorMsg) {
            cerr << type << ' ' << filename << " @ " << line << ": ERROR_NO[" << errNo << "] : " << errorMsg << endl;
        }
    };
}

#endif //RDMA_LOGGING_HPP