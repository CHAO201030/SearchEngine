#include "../include/DictProducer.h"

DictProducer::DictProducer(const string & corpus_path, const string & stop_word_file_path, LanguageType language, SplitTool * tool)
: _language(language), _cut_tool(tool)
{
    readCorpusFiles(corpus_path);
    initStopWordSet(stop_word_file_path);
}

/********************************** 对外接口 **********************************/

void DictProducer::generateAllFiles()
{
    generateDictFile();
    generateDictIndexFile();
}

void DictProducer::generateDictFile()
{
    switch (_language)
    {
    case SE_LT_English: buildEnglishDict(); break;
    case SE_LT_Chinese: buildChineseDict(); break;
    default:
        break;
    }

    storeDictFile();
}

void DictProducer::generateDictIndexFile()
{
    createDictIndex();
    storeDictIndexFile();
}

/******************************** 读取语料辅助函数 ********************************/

void DictProducer::readCorpusFiles(const string corpus_file_dir_path)
{
    DIR * pCorpusdir = opendir(corpus_file_dir_path.c_str());

    if(pCorpusdir == nullptr)
    {
        perror("DictProducer::readCorpusFiles -> opendir");
        exit(-1);
    }

    struct dirent * pdirent = nullptr;

    while((pdirent = readdir(pCorpusdir)) != nullptr)
    {
        if(strcmp(pdirent->d_name, ".") == 0 || strcmp(pdirent->d_name, "..") == 0)
        {
            continue;
        }

        if(pdirent->d_type == DT_DIR)
        {
            readCorpusFiles(corpus_file_dir_path + "/" + pdirent->d_name);
        }
        else
        {
            _files.emplace_back(corpus_file_dir_path + "/" + pdirent->d_name);
        }
    }
}

/*******************************   停用词辅助函数  *******************************/

void DictProducer::initStopWordSet(const string stop_word_file_path)
{
    ifstream ifs(stop_word_file_path);

    string stop_word;
    
    while(ifs >> stop_word)
    {
        _stop_words.insert(stop_word);
    }

    ifs.close();

    cout << "[INFO] : Load Stop Word File -> " << stop_word_file_path << " Success...\n\n";
}

bool DictProducer::isStopWord(const string & word)
{
    return _stop_words.find(word) != _stop_words.end();
}

/******************************** 字典生成辅助函数 ********************************/

void DictProducer::buildEnglishDict()
{
    cout << "[INFO] : Build English Dict begin...\n"
         << "[INFO] : Load Files...\n";

    for(auto & file_name : _files)
    {        
        ifstream ifs(file_name);

        string word;

        map<string, int> temp_dict;

        while(ifs >> word)
        {
            extractEnglishWord(word);

            if(word == "")continue;

            if(!isStopWord(word))
            {
                temp_dict[word]++;
            }
        }

        saveWord(temp_dict);

        ifs.close();

        cout << "\t-> " << file_name << " success\n";
    }

    cout << "[INFO] : Build English Dict Success...\n\n";
}

void DictProducer::buildChineseDict()
{
    cout << "[INFO] : Build Chinese Dict begin...\n"
         << "[INFO] : Load Files...\n";

    for(auto & file_name : _files)
    {
        ifstream ifs(file_name);

        string temp, content;

        while(ifs >> temp)
        {
            extractChineseWord(temp);
            content += temp;
        }

        vector<string> split_res = _cut_tool->cut(content);

        map<string, int> temp_dict;

        for(auto & word : split_res)
        {
            if(!isStopWord(word))
            {
                temp_dict[word]++;
            }
        }

        saveWord(temp_dict);

        ifs.close();

        cout << "\t-> " << file_name << " success\n";
    }

    cout << "[INFO] : Build Chinese Dict Success...\n\n";
}

void DictProducer::createDictIndex()
{
    size_t dict_size = _dict.size();

    for(size_t i = 0; i < dict_size; i++)
    {
        // word = 北大
        // 6Bytes : 1110xxxx xxxxxxxx xxxxxxxx  1110xxxx xxxxxxxx xxxxxxxx

        string word = _dict[i].first;

        size_t ch_num = word.size() / nBytesCode(word[0]);

        for(size_t j = 0, pos = 0; j < ch_num; j++)
        {
            size_t cur_ch_bytes = nBytesCode(word[pos]);

            _index[word.substr(pos, cur_ch_bytes)].insert(i);

            pos += cur_ch_bytes;
        }
    }

    return;
}

void DictProducer::storeDictFile()
{
    ofstream ofs_dict;

    string dict_file_path = "../data/";

    switch (_language)
    {
    case SE_LT_English : dict_file_path += "en_dict.dat"; break;    
    case SE_LT_Chinese : dict_file_path += "zh_dict.dat"; break;
    default:
        break;
    }

    ofs_dict.open(dict_file_path);

    cout << "[INFO] : Store Dict File -> " << dict_file_path << "\n";
    
    for(size_t i = 0; i < _dict.size(); i++)
    {
        ofs_dict << _dict[i].first << " " << _dict[i].second << "\n";
    }

    ofs_dict.close();

    cout << "[INFO] : Store Dict File Success...\n\n";
}

void DictProducer::storeDictIndexFile()
{
    ofstream ofs_dict_index;

    string dict_index_file_path = "../data/";

    switch (_language)
    {
    case SE_LT_English : dict_index_file_path += "en_dict_index.dat"; break;    
    case SE_LT_Chinese : dict_index_file_path += "zh_dict_index.dat"; break;
    default:
        break;
    }

    ofs_dict_index.open(dict_index_file_path);

    cout << "[INFO] : Store Dict Index File -> " << dict_index_file_path << "\n";

    auto mit = _index.begin();

    while(mit != _index.end())
    {
        ofs_dict_index << mit->first;

        for(auto sit : mit->second)
        {
            ofs_dict_index << " " << sit;
        }

        ofs_dict_index << "\n";

        ++mit;
    }

    ofs_dict_index.close();

    cout << "[INFO] : Store Dict Index File Success...\n\n";
}

/****************************** 通用单词处理辅助函数 ********************************/

size_t DictProducer::nBytesCode(char ch)
{
    if(ch & 0x80)
    {
        int bytes = 1;
        
        for(int i = 0; i != 6; i++)
        {
            if(ch & (1 << (6 - i)))
            {
                bytes++;
            }
            else
            {
                break;
            }
        }

        return bytes;
    }

    return 1;
}

void DictProducer::saveWord(map<string, int> & temp_dict)
{
    for(auto & mit : temp_dict)
    {
        _dict.emplace_back(mit);
    }
}

/****************************** 英文单词处理辅助函数 ********************************/

void DictProducer::extractEnglishWord(string & word)
{
    auto it = word.begin();

    while(it != word.end())
    {
        if(isalpha(*it))
        {
            if(isupper(*it))
            {
                *it = tolower(*it);
            }
            ++it;
        }
        else
        {
            it = word.erase(it);
        }
    }
}

/****************************** 中文单词处理辅助函数 ********************************/

bool DictProducer::isChinese(char ch)
{
    return 0x80 & ch;
}

void DictProducer::extractChineseWord(string & word)
{
    for(size_t i = 0; i < word.size();)
    {
        size_t bytes = nBytesCode(word[i]);

        if(!isChinese(word[i]))
        {
            word.erase(i, bytes);
            continue;
        }

        i += bytes;
    }
}
