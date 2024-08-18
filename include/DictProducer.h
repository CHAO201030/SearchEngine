#ifndef __DICT_PRODUCER_H__
#define __DICT_PRODUCER_H__

#include "Configuration.h"
#include "SplitTool.h"

#include <iostream>
#include <vector>
#include <string>
#include <map>
#include <set>
#include <unordered_set>
#include <fstream>
#include <sstream>
#include <sys/types.h>
#include <dirent.h>
#include <string.h>

using std::cin;
using std::cout;
using std::map;
using std::set;
using std::pair;
using std::string;
using std::vector;
using std::unordered_set;
using std::istringstream;
using std::ostringstream;
using std::ifstream;
using std::ofstream;

typedef enum LanguageType{
    // SE : Search Engine
    // LT : Language Type

    SE_LT_English = 0,
    SE_LT_Chinese

}LanguageType;

class DictProducer
{
public:

    /**
     * @brief 构造函数, 加载语料以及停用词文件, 会读取file_path下的所有语料文件
     * 
     * @param corpus_path 存储语料文件的目录路径
     * @param stop_word_file_path 存储停用词文件的目录路径
     * @param language 语言
     * @param tool 分词工具 默认以空格进行分词
     */
    DictProducer(const string & corpus_path,
                 const string & stop_word_file_path,
                 LanguageType language,
                 SplitTool * tool = nullptr);

public:

    /********************************** 对外接口 **********************************/

    /**
     * @brief 生成字典文件和单词-字典位置索引文件
     */
    void generateAllFiles();

    /**
     * @brief 生成字典文件
     */
    void generateDictFile();

    /**
     * @brief 生成单词-字典位置索引文件
     */
    void generateDictIndexFile();

private:

    /******************************** 读取语料辅助函数 ********************************/

    /**
     * @brief 读取所有语料文件的路径
     * 
     * @param corpus_file_dir_path 语料文件目录路径
     */
    void readCorpusFiles(const string corpus_file_dir_path);

private:

    /*******************************   停用词辅助函数  *******************************/

    /**
     * @brief 初始化停用词集合
     * 
     * @param stop_word_file_path 停用词文件的路径
     */
    void initStopWordSet(const string stop_word_file_path);

    /**
     * @brief 判断是否为停用词
     * 
     * @param word 要判断的单词
     * @return true 是停用词
     * @return false 不是停用词
     */
    bool isStopWord(const string & word);

private:
    
    /******************************** 字典生成辅助函数 ********************************/
    
    /**
     * @brief 生成英文字典文件
     */
    void buildEnglishDict();

    /**
     * @brief 生成中文字典文件
     */
    void buildChineseDict();

    /**
     * @brief 创建单词-字典位置索引库
     */
    void createDictIndex();

    /**
     * @brief 存储字典文件
     */
    void storeDictFile();

    /**
     * @brief 存储单词-字典位置索引文件
     */
    void storeDictIndexFile();

private:

    /****************************** 通用单词处理辅助函数 ********************************/
    
    /**
     * @brief 计算单词在UTF-8编码下占几个字节
     * 
     * @param ch 单词的第一个字节
     * @return size_t 单词所占字节数
     */
    size_t nBytesCode(char ch);

    /**
     * @brief 将单词保存在词频字典数据结构中
     * 
     * @param temp_dict 临时字典
     */
    void saveWord(map<string, int> & temp_dict);

    /****************************** 英文单词处理辅助函数 ********************************/

    /**
     * @brief 清洗英文单词, 全部转换为小写且去除所有非英文字母字符
     * 
     * @param word 要处理的单词
     */
    void extractEnglishWord(string & word);

    /****************************** 中文单词处理辅助函数 ********************************/

    /**
     * @brief 判断单词是否为中文字符
     * 
     * @param ch 单词的第一个字节
     * @return true 是中文字符
     * @return false 不是中文字符
     */
    bool isChinese(char ch);

    /**
     * @brief 清洗中文单词, 去除所有非中文字符
     * 
     * @param word 要处理的单词
     */
    void extractChineseWord(string & word);

private:
    LanguageType _language; // 语言类型

    SplitTool * _cut_tool;    // 分词工具

    vector<string> _files;  // 存储语料文件的路径

    vector<pair<string, int>> _dict;    // {word, frequence}词频字典

    map<string, set<int>> _index;   // 位置索引 存储词和词在字典中出现的位置信息(vector的索引)
    
    unordered_set<string> _stop_words;  // 停用词集合
};

#endif
