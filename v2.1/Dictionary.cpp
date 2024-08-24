#include "../include/Dictionary.h"

Dictionary * Dictionary::_p_dictionary = nullptr;

Dictionary::Dictionary()
{
    _p_split_tool = new SplitToolCppJieba();
    loadDataSet();
    loadWebPageFile();
}

Dictionary * Dictionary::getInstance()
{
    if(_p_dictionary == nullptr)
    {
        _p_dictionary = new Dictionary();
    }

    return _p_dictionary;
}

void Dictionary::delInstance()
{
    if(_p_dictionary)
    {
        delete _p_dictionary;
        _p_dictionary = nullptr;
    }
}

void Dictionary::loadDataSet()
{
    ifstream en_dict_ifs(Configuration::getInstance()->getConfig("english_dict.dat"));
    ifstream zh_dict_ifs(Configuration::getInstance()->getConfig("chinese_dict.dat"));

    ifstream en_index_ifs(Configuration::getInstance()->getConfig("english_dict_index.dat"));
    ifstream zh_index_ifs(Configuration::getInstance()->getConfig("chinese_dict_index.dat"));

    string line;

    // 1. 加载英文字典
    while(getline(en_dict_ifs, line))
    {
        istringstream iss(line);
        
        string word;
        
        int frequence;

        iss >> word >> frequence;
    
        _dict_freq_vec.emplace_back(word, frequence);
    }

    // 字典偏移英文单词数
    int en_offset = _dict_freq_vec.size();

    // 2. 加载中文字典
    while(getline(zh_dict_ifs, line))
    {
        istringstream iss(line);
        
        string word;
        
        int frequence;

        iss >> word >> frequence;
    
        _dict_freq_vec.emplace_back(word, frequence);
    
    }

    // 3. 加载英文索引
    while(getline(en_index_ifs, line))
    {
        istringstream iss(line);

        string word;
        
        int index;

        iss >> word;

        while(iss.good())
        {
            iss >> index;

            _word_index_map[word].insert(index);
        }
    }

    // 4. 加载中文索引
    while(getline(zh_index_ifs, line))
    {
        istringstream iss(line);

        string word;
        
        int index;

        iss >> word;

        while(iss.good())
        {
            iss >> index;

            _word_index_map[word].insert(index + en_offset);
        }
    }
}

void Dictionary::loadWebPageFile()
{
    ifstream unrepeated_offset_lib_ifs(Configuration::getInstance()->getConfig("unrepeated_offsetlib.dat"));
    ifstream invert_index_lib_ifs(Configuration::getInstance()->getConfig("invert_index_lib.dat"));

    string line;
    while(getline(unrepeated_offset_lib_ifs, line))
    {
        istringstream iss(line);
        
        int doc_id = 0, offset = 0, page_len = 0;

        iss >> doc_id >> offset >> page_len;

        _offset_lib_ves.emplace_back(offset, page_len);
    }

    while(getline(invert_index_lib_ifs, line))
    {
        istringstream iss(line);

        string word;

        int doc_id = 0;

        double weight = 0;

        iss >> word;

        while(!iss.eof())
        {
            iss >> doc_id >> weight;

            _invert_index_lib[word][doc_id] = weight;
        }
    }
}

vector<pair<string, int>> & Dictionary::getWordFrequenceDict()
{
    return _dict_freq_vec;
}

map<string, set<int>> & Dictionary::getWordFequenceIndexMap()
{
    return _word_index_map;
}

vector<pair<int, int>> & Dictionary::getOffsetLib()
{
    return _offset_lib_ves;
}

unordered_map<string, unordered_map<int, double>> & Dictionary::getInvertIndexLib()
{
    return _invert_index_lib;
}

SplitTool * Dictionary::getSpliTool()
{
    return _p_split_tool;
}
