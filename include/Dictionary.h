#ifndef __DICTIONARY_H__
#define __DICTIONARY_H__

#include "Configuration.h"
#include "SplitToolCppJieba.h"
#include <vector>
#include <set>
#include <map>
#include <string>
#include <fstream>
#include <sstream>
#include <iostream>
#include <unordered_map>
#include <utility>

using std::vector;
using std::string;
using std::map;
using std::unordered_map;
using std::pair;
using std::set;
using std::ifstream;
using std::ofstream;
using std::istringstream;
using std::ostringstream;

class Dictionary
{
public:
    static Dictionary * getInstance();

    static void delInstance();

    vector<pair<string, int>> & getWordFrequenceDict();

    map<string, set<int>> & getWordFequenceIndexMap();

    vector<pair<int, int>> & getOffsetLib();

    unordered_map<string, unordered_map<int, double>> & getInvertIndexLib();

    SplitTool * getSpliTool();

private:
    Dictionary();

    Dictionary(const Dictionary & rhs) = delete;
    Dictionary(Configuration && rhs) = delete;

    Dictionary & operator=(const Dictionary & rhs) = delete;
    Dictionary & operator=(Dictionary && rhs)      = delete;

private:
    void loadDataSet();
    void loadWebPageFile();
private:
    static Dictionary * _p_dictionary;

    SplitTool *_p_split_tool = nullptr; // 分词工具

    vector<pair<string, int>> _dict_freq_vec;   // 词频字典

    map<string, set<int>> _word_index_map;  // 词频字典索引表

    vector<pair<int, int>> _offset_lib_ves; // 去重网页偏移库

    unordered_map<string, unordered_map<int, double>> _invert_index_lib;    // 去重网页倒排索引库
};

#endif
