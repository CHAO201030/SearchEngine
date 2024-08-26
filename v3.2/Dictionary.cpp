#include "../include/Dictionary.h"

#include <mysql++/connection.h>
#include <mysql++/query.h>
#include <mysql++/row.h>
#include <mysql++/result.h>

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
    mysqlpp::Connection conn("SearchEngine", "192.168.239.128", "root", "1234", 3306);
    
    mysqlpp::Query query = conn.query();

    // 1. 加载英文字典
    query << "SELECT word, frequence FROM en_dict";

    mysqlpp::UseQueryResult query_res = query.use();

    if(query_res)
    {
        while(mysqlpp::Row row = query_res.fetch_row())
        {
            string word(row["word"]);

            int frequence = std::stoi(string(row["frequence"]));

            _dict_freq_vec.emplace_back(word, frequence);
        }
    }

    int en_offset = _dict_freq_vec.size();

    // 2. 加载中文字典
    query << "SELECT word, frequence FROM zh_dict";

    query_res = query.use();

    if(query_res)
    {
        while(mysqlpp::Row row = query_res.fetch_row())
        {
            string word(row["word"]);

            int frequence = std::stoi(string(row["frequence"]));

            _dict_freq_vec.emplace_back(word, frequence);
        }
    }

    // 3. 加载英文索引
    query << "SELECT word, idx FROM en_index_map";

    query_res = query.use();

    if(query_res)
    {
        while(mysqlpp::Row row = query_res.fetch_row())
        {
            string word(row["word"]);

            int index = std::stoi(string(row["idx"]));

            _word_index_map[word].insert(index);
        }
    }

    // 4. 加载中文索引
    query << "SELECT word, idx FROM zh_index_map";
    
    query_res = query.use();

    if(query_res)
    {
        while(mysqlpp::Row row = query_res.fetch_row())
        {
            string word(row["word"]);

            int index = std::stoi(string(row["idx"]));

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
