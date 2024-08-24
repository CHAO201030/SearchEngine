#include "../include/Configuration.h"

Configuration * Configuration::_ptr = nullptr;

Configuration::Configuration()
{

}

void Configuration::readConfigurationFile()
{
    ifstream cfg_ifs("../config/myconf.conf");
    // ifstream cfg_ifs("../config/test.conf");

    string line;
    
    while(getline(cfg_ifs, line))
    {
        istringstream iss(line);
    
        string key, equal, value;

        iss >> key >> equal >> value;

        _cfg.insert({key, value});
    }
}

Configuration * Configuration::getInstance()
{
    if(_ptr == nullptr)
    {
        _ptr = new Configuration();
    }

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
    if(_cfg.size() == 0)
    {
        readConfigurationFile();
    }

    auto umit = _cfg.find(key);

    if(umit == _cfg.end())
    {
        return "";
    }

    return umit->second;
}

unordered_set<string> & Configuration::getStopWordSet()
{
    if(_stop_words.size() == 0)
    {
        readStopWordFile();
    }

    return _stop_words;
}

void Configuration::readStopWordFile()
{
    ifstream zh_stop_word_ifs(_cfg["chinese_stop_words_file"]);
    ifstream en_stop_word_ifs(_cfg["english_stop_words_file"]);

    string stop_word;

    while(en_stop_word_ifs >> stop_word)
    {
        _stop_words.insert(stop_word);
    }

    while(zh_stop_word_ifs >> stop_word)
    {
        _stop_words.insert(stop_word);
    }

    zh_stop_word_ifs.close();
    en_stop_word_ifs.close();
}

// // Test Unit    ----> PASS
// int main(void)
// {
//     string test = Configuration::getInstance()->getConfig("test_en.txt");
//     string nu = Configuration::getInstance()->getConfig("asdasd");
//
//     std::cout << Configuration::getInstance()->getStopWordSet().size() << "\n";
//     return 0;
// }
