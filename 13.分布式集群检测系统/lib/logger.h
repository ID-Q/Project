/*************************************************************************
	> File Name: logger.h
	> Author: 
	> Mail: 
	> Created Time: 三 10/ 7 10:12:20 2020
 ************************************************************************/

#ifndef _LOGGER_H
#define _LOGGER_H

#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <ctime>
#include <map>

#define LOG_TRACE(level) log::logger(__FILE__, __func__, __LINE__, level)
#define LOG_INFO LOG_TRACE(log::LogLevel::INFO)
#define LOG_DEBUG LOG_TRACE(log::LogLevel::DEBUG)
#define LOG_WARNING LOG_TRACE(log::LogLevel::WARNING)
#define LOG_ERROR LOG_TRACE(log::LogLevel::ERROR)
#define LOG_SET_LEVEL(level) log::Logger::set_level(level)
#define LOG_SET_TARGET(target, path) log::Logger::set_target(target, path)

namespace log {
class LogLevel {
public:
    enum Levels { INFO = 1, DEBUG = 2, WARNING = 4, ERROR = 8 }; // 日志水平
    enum Targets { FILES = 1, TERMINAL = 2}; // 输出位置
};

std::map<int, std::string> LogLevels {
    { LogLevel::Levels::INFO, "info" },     
    { LogLevel::Levels::DEBUG, "debug" },
    { LogLevel::Levels::WARNING, "warning" },      
    { LogLevel::Levels::ERROR, "error" }     
};

std::string log_time(){
    char tmp[64];
    time_t ltime;
    time(&ltime);
    strftime(tmp, sizeof(tmp), "%Y-%m-%d %H:%M:%S", localtime(&ltime));
    return tmp;
}

class Logger {
    class LoggerStream : public std::ostringstream {
    public:
        LoggerStream(const char *file, const char *func, int line, Logger &obj, int level) : _obj(obj), _flag(1) {
            std::ostringstream &now = *this;
            now << "[" << file << " project] ";
            now << "[" << func << "] ";
            now << "[" << log_time() << "] ";
            now << "[" << LogLevels[level] << "] ";
            now << "(" << line << ") :";
            _flag = Logger::_level & level;
        }
        LoggerStream(const LoggerStream &ls) : _obj(ls._obj) {}
        ~LoggerStream() {
            if (_flag) output();
        }
    private:
        void output() {
            std::unique_lock<std::mutex> lock(_obj._mutex);
            std::ostringstream &now = *this;
            switch (_obj._target) {
            case LogLevel::Targets::FILES:
                this->_ofile.open(_obj._path, std::ios::out | std::ios::app);
                this->_ofile << this->str() << std::endl;
                std::cout.flush();
                this->_ofile.close();
                break;
            case LogLevel::Targets::TERMINAL:
                std::cout << this->str() << std::endl;
                std::cout.flush();
                break;
            default:
                std::cout << this->str() << std::endl;
                std::cout.flush();
                break;
            }
            return ;
        }
        Logger &_obj;
        std::ofstream _ofile;
        int _flag;
    };
public:
    LoggerStream operator()(const char *file, const char *func, int line, int level) {
        return LoggerStream(file, func, line, *this, level);
    }
    static void set_level(int level) {
        Logger::_level = level;
        return ;
    }
    static void set_target(int target, const char *path) {
        Logger::_target = target;
        Logger::_path = path;
        return ;
    }
private:
    std::mutex _mutex;
    static int _level;
    static int _target;
    static const char *_path;
};

int Logger::_level = 15;
int Logger::_target = 15;
const char *Logger::_path = "./";
Logger logger;

}

#endif
