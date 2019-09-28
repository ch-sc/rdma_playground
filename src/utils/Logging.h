#ifndef RDMA_LOGGING_HPP
#define RDMA_LOGGING_HPP

#include "../config/Config.h"

#include <iostream>
#include <string>
#include <cerrno>

namespace rdma {

    class Logging {
    public:
        static void debug(const std::string& msg) {
            // avoid unused variable warning
            (void) msg;
#ifdef DEBUG
            if (Config::LOGGING_LEVEL <= LOG_LEVEL_DEBUG)
                log("[DEBUG]: ", msg);
#endif
        }

        static void debug(const std::string& filename, int line, const std::string& msg) {
            // avoid unused variable warning
            (void) filename;
            (void) line;
            (void) msg;
#ifdef DEBUG
            if (Config::LOGGING_LEVEL <= LOG_LEVEL_DEBUG)
                log("[DEBUG]: ", filename, line, msg);
#endif
        }

        static void info(const std::string& msg) {
            if (Config::LOGGING_LEVEL <= LOG_LEVEL_INFO) {
                log("[INFO]", msg);
            }
        }

        static void info(const std::string& filename, int line, const std::string& msg) {
            if (Config::LOGGING_LEVEL <= LOG_LEVEL_INFO) {
                log("[INFO]", filename, line, msg);
            }
        }

        static void warn(const std::string& msg) {
            if (Config::LOGGING_LEVEL <= LOG_LEVEL_WARNING) {
                log("[WARNING]", msg);
            }
        }

        static void warn(const std::string& filename, int line, const std::string& msg) {
            if (Config::LOGGING_LEVEL <= LOG_LEVEL_WARNING) {
                log("[WARNING]", filename, line, msg);
            }
        }

        static void error(const std::string& filename, int line, const std::string& msg) {
            if (Config::LOGGING_LEVEL <= LOG_LEVEL_WARNING) {
                log("[ERROR]", filename, line, msg);
            }
        }

        static void errorNo(const std::string& filename, int line, char *msg, int err_no) {
            if (Config::LOGGING_LEVEL <= LOG_LEVEL_ERROR)
                logNo("[ERROR]", filename, line, err_no, msg);
        }

        static void fatal(const std::string& filename, int line, const std::string& msg) {
            if (Config::LOGGING_LEVEL <= LOG_LEVEL_ERROR)
                log("[FATAL]: ", filename, line, msg);
        }


    private:
        static void log(const std::string& type, const std::string& msg) {
            cerr << type << ' ' << msg << endl;
        }

        static void log(const std::string& type, const std::string& filename, int line, const std::string& msg) {
            cerr << type << ' ' << filename << " @ " << line << ":" << msg << endl;
        }

        static void logNo(const std::string& type, const std::string& filename, int line, int errNo, char *errorMsg) {
            cerr << type << ' ' << filename << " @ " << line << ": ERROR_NO[" << errNo << "] : " << errorMsg << endl;
        }
    };
}

#endif //RDMA_LOGGING_HPP