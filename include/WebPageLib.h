#ifndef __WEB_PAGE_LIB_H__
#define __WEB_PAGE_LIB_H__

#include "DirScanner.h"
#include <map>
#include <iostream>
#include <sstream>
#include <fstream>
#include <tinyxml2.h>
#include <regex>

using std::regex;
using std::cout;
using std::ofstream;
using std::ifstream;
using std::ostringstream;
using std::vector;
using std::string;
using std::map;
using std::pair;
using tinyxml2::XMLDocument;
using tinyxml2::XMLElement;

class WebPageLib
{
public:
    void create();

    void store();

private:
    void processWebPage(const string file_path);

    void getRssItem(XMLElement * item_element, string & title, string & link, string & content);

    void deleteHTMLTag(string & content);

    string generateWebPage(string & title, string & link, string & content);

private:
    DirScanner _dir_scanner;    // 读取目录扫描文件

    vector<string> _web_pages;    // 网页

    map<int, pair<int, int>> _offset_lib; // 偏移库 {网页ID, {起始位置, 结束位置}}
};

#endif
