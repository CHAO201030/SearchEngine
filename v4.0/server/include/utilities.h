#ifndef __UTILITIES_H__
#define __UTILITIES_H__

#include<iostream>
#include<fstream>
#include<sstream>
#include<algorithm>
#include<map>
#include<set>
#include<queue>
#include<vector>
#include<string>
#include<utility>
#include<unordered_set>
#include<unordered_map>
#include "./simhash/cppjieba/Jieba.hpp"

using std::cin;
using std::cout;
using std::map;
using std::set;
using std::pair;
using std::vector;
using std::string;
using std::unordered_set;
using std::unordered_map;
using std::priority_queue;

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
    Configuration(Configuration && rhs) = delete;
    Configuration & operator=(const Configuration & rhs) = delete;
    Configuration & operator=(Configuration && rhs) = delete;

private:
    static Configuration * _ptr;
    unordered_set<string> _stop_words;
    unordered_map<string, string> _cfg;
};


class SplitTool
{
public:
    SplitTool();
    ~SplitTool();
    vector<string> cut(const string & str);

private:
    cppjieba::Jieba * _jieba = nullptr;
};


class Dictionary
{
public:
    static Dictionary * getInstance();
    static void delInstance();
    vector<pair<string, int>> & getWordFreqDict();
    map<string, set<int>> & getWordFreqIndexMap();
    vector<pair<int, int>> & getOffsetLib();
    unordered_map<string, unordered_map<int, double>> & getInvertIndexLib();
    SplitTool * getSplitTool();

private:
    Dictionary();
    ~Dictionary();
    Dictionary(const Dictionary & rhs) = delete;
    Dictionary(Dictionary && rhs) = delete;
    Dictionary & operator=(const Dictionary & rhs) = delete;
    Dictionary & operator=(Dictionary && rhs) = delete;

    void loadDict();
    void loadWebPageFile();

private:
    static Dictionary * _ptr;
    vector<pair<string, int>> _word_freq_vec;
    map<string, set<int>> _word_idx_map;
    vector<pair<int, int>> _offset_lib_vec;
    unordered_map<string, unordered_map<int, double>> _invert_index_lib;
    SplitTool * _p_split_tool;
};

#endif