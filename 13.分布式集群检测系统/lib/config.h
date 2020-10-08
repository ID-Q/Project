/*************************************************************************
	> File Name: config.h
	> Author: 
	> Mail: 
	> Created Time: 二 10/ 6 17:33:12 2020
 ************************************************************************/

#ifndef _CONFIG_H
#define _CONFIG_H

#include <iostream>  
#include <fstream>  
#include <sstream>  
#include <string>  
#include <map>  

class Config {  
public:  
    Config(std::string filename, std::string symbol = "=", std::string comment = "#");  
    Config();  
    template<class T> 
    T read(const std::string& in_key) const; // 搜索键和读值或可选的默认值，调用为读  
    template<class T> 
    T read(const std::string& in_key, const T& in_value) const;  
    template<class T> 
    bool read_into(T& out_var, const std::string& in_key) const;  
    template<class T>  
    bool read_into(T& out_var, const std::string& in_key, const T& in_value) const;  
    bool file_exist(std::string filename);  
    void read_file(std::string filename, std::string symbol = "=", std::string comment = "#");  
  
    // 检查配置中是否存在键值 
    bool key_exists(const std::string& in_key) const;  
  
    // 修改键和值 
    template<class T> 
    void add(const std::string& in_key, const T& in_value);  
    void remove(const std::string& in_key);  
  
    // 检查或更改配置语法 
    std::string get_symbol() const { return _symbol; }  
    std::string get_comment() const { return _comment; }  
    std::string set_symbol(const std::string& in_s) { 
        std::string old = _symbol; 
        _symbol = in_s; 
        return old; 
    }    
    std::string set_comment(const std::string& in_s) {
        std::string old = _comment;  
        _comment = in_s;  
        return old; 
    }  
  
    // 写入或读取配置 
    friend std::ostream& operator<<(std::ostream& os, const Config& cf);  
    friend std::istream& operator>>(std::istream& is, Config& cf);  
    // 异常类型 
    struct file_not_found {  
        std::string filename;  
        file_not_found(const std::string& filename_ = std::string()) : filename(filename_) {} 
    };  
    struct key_not_found {  // thrown only by T read(key) variant of read()  
        std::string key;  
        key_not_found(const std::string& key_ = std::string()) : key(key_) {} 
    };  
private:  
    std::string _symbol; // 键和值之间的分隔符 
    std::string _comment; // 值和注释之间的分隔符  
    std::map<std::string,std::string> _content; // 提取的键和值  
    typedef std::map<std::string, std::string>::iterator _mapi;  
    typedef std::map<std::string, std::string>::const_iterator _mapci;  
    template<class T> 
    static std::string T_as_string(const T& t);  
    template<class T> 
    static T string_as_T(const std::string& s);  
    static void Trim(std::string& inout_s);  
};  

/* static */  
template<class T>  
std::string Config::T_as_string(const T& t) { 
    // 从一个T转换为一个字符串 
    // 类型T必须支持<<操作符   
    std::ostringstream ost;  
    ost << t;  
    return ost.str();  
}  

/* static */  
template<class T>  
T Config::string_as_T(const std::string& s) {  
    // 从一个字符串转换为一个T 
    // 类型T必须支持>>操作符 
    T t;  
    std::istringstream ist(s);  
    ist >> t;  
    return t;  
}  

/* static */  
template<>  
inline std::string Config::string_as_T<std::string>(const std::string& s) {  
    // 从一个字符串转换为一个字符串
    // 换句话说，什么也不做
    return s;  
}  
  
/* static */  
template<>  
inline bool Config::string_as_T<bool>(const std::string& s) {  
    // 从一个字符串转换为一个bool
    // 把“假”、“F”、“否”、“n”、“0”解释为假
    // 把“真”，“T”，“是”，“y”，“1”，“-1”，或其他任何为真
    bool b = true;  
    std::string sup = s;  
    for (std::string::iterator p = sup.begin(); p != sup.end(); ++p)  
        *p = toupper(*p);  // 全大写 
    if (sup == std::string("FALSE") || sup == std::string("F") ||  
        sup == std::string("NO") || sup == std::string("N") ||  
        sup == std::string("0") || sup == std::string("NONE") )  
        b = false;  
    return b;  
}  

template<class T>  
T Config::read(const std::string& key) const {  
    // 读取与key对应的值 
    _mapci p = _content.find(key);  
    if (p == _content.end()) throw key_not_found(key);  
    return string_as_T<T>(p->second);  
}  
  
template<class T>  
T Config::read(const std::string& key, const T& value) const {  
    // 返回对应于key或给定默认值的值
    // 如果没有找到key
    _mapci p = _content.find(key);  
    if (p == _content.end()) return value;  
    return string_as_T<T>(p->second);  
}  

template<class T>  
bool Config::read_into(T& var, const std::string& key) const {  
    // 获取key对应的值并存储在var中 
    // 如果找到键返回真
    // 否则保持var不变 
    _mapci p = _content.find(key);  
    bool found = (p != _content.end());  
    if(found) var = string_as_T<T>(p->second);  
    return found;  
}  

template<class T>  
bool Config::read_into(T& var, const std::string& key, const T& value) const {  
    // 获取key对应的值并存储在var中 
    // 如果找到键返回真
    // 否则将var设置为给定的默认值
    _mapci p = _content.find(key);  
    bool found = (p != _content.end());  
    if (found) {  
        var = string_as_T<T>(p->second);  
    } else {  
        var = value;  
    }
    return found;  
}  

template<class T>  
void Config::add(const std::string& in_key, const T& value) {  
    // 添加具有给定值的键 
    std::string v = T_as_string(value);  
    std::string key = in_key;  
    Trim(key);  
    Trim(v);  
    _content[key] = v;  
    return;  
}  

Config::Config(std::string filename, std::string delimiter, std::string comment) : _symbol(delimiter), _comment(comment) {  
    // 构造一个配置，从给定的文件中获取键和值
    std::ifstream in(filename.c_str());  
    if(!in) throw file_not_found(filename);   
    in >> (*this);  
}  
  
Config::Config() : _symbol(std::string(1,'=')), _comment(std::string(1,'#')) {  
    // 构造不带文件的配置;空
}  
  
bool Config::key_exists(const std::string& key) const {  
    // 指示是否找到key
    _mapci p = _content.find(key);  
    return (p != _content.end());  
}  
  
/* static */  
void Config::Trim(std::string& inout_s) {  
    // 删除前导和末尾的空格  
    static const char whitespace[] = " \n\t\v\r\f";  
    inout_s.erase(0, inout_s.find_first_not_of(whitespace));  
    inout_s.erase(inout_s.find_last_not_of(whitespace) + 1U);  
}  
  
std::ostream& operator<<(std::ostream& os, const Config& cf) {  
    // 将配置保存到输出流
    for (Config::_mapci p = cf._content.begin(); p != cf._content.end(); ++p) {  
        os << p->first << " " << cf._symbol << " ";  
        os << p->second << std::endl;  
    }  
    return os;  
}  
  
void Config::remove(const std::string& key) {  
    // 删除键及其值
    _content.erase(_content.find(key));  
    return;  
}  
  
std::istream& operator>>(std::istream& is, Config& cf) {  
    // 从输入流加载一个配置  
    // 读取键和值，保持内部空白
    typedef std::string::size_type pos;  
    const std::string& symbol = cf._symbol; // 标识符 
    const std::string& comment = cf._comment; // 注释  
    const pos skip = symbol.length(); // 标识符长度  
  
    std::string nextline = "";  // 可能需要提前阅读以了解值在哪里结束
  
    while (is || nextline.length() > 0) {  
        // 一次读一整行  
        std::string line;  
        if (nextline.length() > 0) {  
            line = nextline; // 我们提前阅读;现在使用它 
            nextline = "";  
        } else {  
            std::getline(is, line);  
        }  
        // 忽略注释
        line = line.substr(0, line.find(comment));  
  
        // 如果该行包含分隔符，请解析该行 
        pos delimPos = line.find(symbol);  
        if (delimPos < std::string::npos) {  
            // 提取的键值
            std::string key = line.substr(0, delimPos);  
            line.replace(0, delimPos + skip, "");  
  
            // 看看value是否在下一行继续
            // 停留在空白行，下一行有键，结束流 
            // 或文件结束哨兵
            bool terminate = false;  
            while (!terminate && is) {  
                std::getline(is, nextline);  
                terminate = true;  
                std::string nlcopy = nextline;  
                Config::Trim(nlcopy);  
                if (nlcopy == "") continue;  
  
                nextline = nextline.substr(0, nextline.find(comment));  
                if (nextline.find(symbol) != std::string::npos)  
                    continue;  
  
                nlcopy = nextline;  
                Config::Trim(nlcopy);  
                if (nlcopy != "") line += "\n";  
                line += nextline;  
                terminate = false;  
            }  
  
            // 存储键和值
            Config::Trim(key);  
            Config::Trim(line);  
            cf._content[key] = line;  // 如果密钥重复，则重写
        }  
    }  
    return is;  
}

bool Config::file_exist(std::string filename) {  
    bool exist= false;  
    std::ifstream in(filename.c_str());  
    if (in) exist = true;  
    return exist;  
}  
  
void Config::read_file(std::string filename, std::string delimiter, std::string comment) {  
    _symbol = delimiter;  
    _comment = comment;  
    std::ifstream in(filename.c_str());  
  
    if (!in) throw file_not_found(filename);   
    in >> (*this);  
}

Config cf;
#endif
