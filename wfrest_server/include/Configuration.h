#ifndef __CONFIGURATION_H__
#define __CONFIGURATION_H__

#include <memory>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <iostream>
#include <fstream>
#include <sstream>

using std::string;
using std::unordered_map;
using std::shared_ptr;
using std::unordered_set;
using std::cin;
using std::ifstream;
using std::istringstream;

class Configuration
{
public:
    static Configuration * getInstance();
    
    static void delInstance();

    string getConfig(string key);

    unordered_set<string> & getStopWordSet();

private:
    Configuration();

    Configuration(const Configuration & rhs) = delete;
    Configuration(Configuration && rhs)      = delete;

    Configuration & operator=(const Configuration & rhs) = delete;
    Configuration & operator=(Configuration && rhs)      = delete;

private:
    void readConfigurationFile();

    void readStopWordFile();

private:
    static Configuration * _ptr;

    unordered_map<string, string> _cfg;

    unordered_set<string> _stop_words;
};

#endif
