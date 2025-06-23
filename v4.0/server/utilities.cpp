#include "./include/utilities.h"


Configuration * Configuration::_ptr = nullptr;

Configuration * Configuration::getInstance()
{
    if(_ptr == nullptr) _ptr = new Configuration();
    return _ptr;
}

void Configuration::delInstance()
{
    if(_ptr)
    {
        delete _ptr;
        _ptr = nullptr;
    }
}

string Configuration::getConfig(string key)
{
    auto _cfg_it = _cfg.find(key);

    if(_cfg_it != _cfg.end())
    {
        return _cfg_it->second;
    }
    else
    {
        return nullptr;
    }
}

unordered_set<string> & Configuration::getStopWordSet()
{
    return _stop_words;
}

Configuration::Configuration()
{
    std::ifstream ifs("../config/server.cfg");
    string line;

    while(getline(ifs, line))
    {
        auto sharp_pos = line.find("#");
        if(sharp_pos != string::npos)
        {
            line.erase(sharp_pos);
        }

        if(line.empty())continue;

        std::istringstream iss(line);
        string key, equal, value;

        iss >> key >> equal >> value;

        _cfg[key] = value;
    }

    std::ifstream en_stop_words_ifs(_cfg["en_stop_words"]);
    std::ifstream zh_stop_words_ifs(_cfg["zh_stop_words"]);

    while(en_stop_words_ifs >> line)
    {
        _stop_words.insert(line);
    }

    while(zh_stop_words_ifs >> line)
    {
        _stop_words.insert(line);
    }
}


SplitTool::SplitTool()
{
    string DICT_PATH = Configuration::getInstance()->getConfig("DICT_PATH");
    string HMM_PATH = Configuration::getInstance()->getConfig("HMM_PATH");
    string USER_DICT_PATH = Configuration::getInstance()->getConfig("USER_DICT_PATH");
    string IDF_PATH = Configuration::getInstance()->getConfig("IDF_PATH");
    string STOP_WORD_PATH = Configuration::getInstance()->getConfig("STOP_WORD_PATH");

    _jieba = new cppjieba::Jieba(DICT_PATH, HMM_PATH, USER_DICT_PATH, IDF_PATH, STOP_WORD_PATH);
}

SplitTool::~SplitTool()
{
    if(_jieba)
    {
        delete _jieba;
        _jieba = nullptr;
    }
}

vector<string> SplitTool::cut(const string & str)
{
    vector<string> ret;

    _jieba->Cut(str, ret);

    return ret;
}


Dictionary * Dictionary::_ptr = nullptr;

Dictionary * Dictionary::getInstance()
{
    if(_ptr == nullptr) _ptr = new Dictionary();
    return _ptr;
}

void Dictionary::delInstance()
{
    if(_ptr)
    {
        delete _ptr;
        _ptr = nullptr;
    }
}

vector<pair<string, int>> & Dictionary::getWordFreqDict()
{
    return _word_freq_vec;
}

map<string, set<int>> & Dictionary::getWordFreqIndexMap()
{
    return _word_idx_map;
}

vector<pair<int, int>> & Dictionary::getOffsetLib()
{
    return _offset_lib_vec;
}

unordered_map<string, unordered_map<int, double>> & Dictionary::getInvertIndexLib()
{
    return _invert_index_lib;
}

SplitTool * Dictionary::getSplitTool()
{
    return _p_split_tool;
}

Dictionary::Dictionary()
{
    loadDict();
    loadWebPageFile();
    _p_split_tool = new SplitTool();
}

Dictionary::~Dictionary()
{
    if(_p_split_tool)
    {
        delete _p_split_tool;
        _p_split_tool = nullptr;
    }
}

void Dictionary::loadDict()
{
    std::ifstream en_dict_ifs(Configuration::getInstance()->getConfig("en_dict"));
    std::ifstream zh_dict_ifs(Configuration::getInstance()->getConfig("zh_dict"));
    std::ifstream en_dict_idx_ifs(Configuration::getInstance()->getConfig("en_dict_index"));
    std::ifstream zh_dict_idx_ifs(Configuration::getInstance()->getConfig("zh_dict_index"));

    string line;
    while(getline(en_dict_ifs, line))
    {
        std::istringstream iss(line);

        string word;
        int freq;

        iss >> word >> freq;

        _word_freq_vec.emplace_back(word, freq);
    }

    int zh_begin_pos = _word_freq_vec.size();

    while(getline(zh_dict_ifs, line))
    {
        std::istringstream iss(line);

        string word;
        int freq;

        iss >> word >> freq;

        _word_freq_vec.emplace_back(word, freq);
    }

    while(getline(en_dict_idx_ifs, line))
    {
        std::istringstream iss(line);

        string word;
        int idx;

        iss >> word;

        while(iss.good() && !iss.eof())
        {
            iss >> idx;

            _word_idx_map[word].insert(idx);
        }
    }

    while(getline(zh_dict_idx_ifs, line))
    {
        std::istringstream iss(line);

        string word;
        int idx;

        iss >> word;

        while(iss.good() && !iss.eof())
        {
            iss >> idx;

            _word_idx_map[word].insert(zh_begin_pos + idx);
        }
    }
}

void Dictionary::loadWebPageFile()
{
    std::ifstream off_lib_ifs(Configuration::getInstance()->getConfig("offset_lib"));
    std::ifstream invert_idx_ifs(Configuration::getInstance()->getConfig("invert_index_lib"));

    string line;

    while(getline(off_lib_ifs, line))
    {
        std::istringstream iss(line);

        int doc_id, begin_pos, page_len;
            
        iss >> doc_id >> begin_pos >> page_len;

        _offset_lib_vec.emplace_back(begin_pos, page_len);
    }

    while(getline(invert_idx_ifs, line))
    {
        std::istringstream iss(line);

        string word;

        iss >> word;
        
        while(iss.good() && !iss.eof())
        {
            int doc_id;
            double weight;

            iss >> doc_id >> weight;

            _invert_index_lib[word][doc_id] = weight;
        }
    }
}