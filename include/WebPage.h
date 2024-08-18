#ifndef __WEB_PAGE_H__
#define __WEB_PAGE_H__

#include "Configuration.h"
#include "SplitTool.h"
#include "simhash/Simhasher.hpp"
#include <iostream>
#include <sstream>
#include <string>
#include <vector>
#include <map>
#include <tinyxml2.h>

using std::vector;
using std::string;
using std::map;
using simhash::Simhasher;
using tinyxml2::XMLDocument;
using tinyxml2::XMLElement;

class WebPage
{
public:
    WebPage(string & page);

    void unrepeatedWebPageLibHelper(Simhasher * p_simhasher);

    void invertIndexLibHelper(SplitTool * p_split_tool);

    int getID();

    string getContent();
    
    string getTitle();
    
    string getURL();

    map<string, int> & getWordFrequenceMap();

    uint64_t getFigurePrint();

private:
    // 处理网页模块
    void extractWebPage();

    // 生成网页词频模块
    void generateWordFrequenceMap(SplitTool * p_split_tool);

    // 生成网页指纹模块
    void generateFigurePrint(Simhasher * p_simhasher);

private:

    /********************* 中文处理 *********************/
    size_t nBytesCode(char ch);
    
    bool isChinese(char ch);

    void extractChineseWord(string & word);

    bool isStopWord(string & word);

public:
    friend bool operator==(const WebPage & lhs, const WebPage & rhs);

    friend bool operator!=(const WebPage & lhs, const WebPage & rhs);

    friend bool operator<(const WebPage & lhs, const WebPage & rhs);

private:

private:
    int _id;    // 文章ID

    uint64_t _figure_print; // 文章 Simhash 指纹

    string _url;    // 文章URL

    string _title;  // 文章标题

    string _content;    // 文章内容

    string _format_xml; // 初步清洗格式化后的网页内容

    map<string, int> _word_frequence;   // 词频表
};

#endif
