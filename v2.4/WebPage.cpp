#include "../include/WebPage.h"
#include "../include/SplitToolCppJieba.h"

WebPage::WebPage(string & page)
:_format_xml(page)
{

}

void WebPage::unrepeatedWebPageLibHelper(Simhasher * p_simhasher)
{
    extractWebPage();
    generateFigurePrint(p_simhasher);
}

void WebPage::invertIndexLibHelper(SplitTool * p_split_tool)
{
    extractWebPage();
    generateWordFrequenceMap(p_split_tool);
}

int WebPage::getID()
{
    return _id;
}

string WebPage::getTitle()
{
    return _title;
}
    
string WebPage::getURL()
{
    return _url;
}

string WebPage::getContent()
{
    return _content;
}

map<string, int> & WebPage::getWordFrequenceMap()
{
    return _word_frequence;
}

uint64_t WebPage::getFigurePrint()
{
    return _figure_print;
}

void WebPage::extractWebPage()
{
    XMLDocument doc;
    doc.Parse(_format_xml.c_str());
    
    XMLElement * page_elem    = doc.FirstChildElement("webpage");

    
    if(page_elem == nullptr)
    {
        // 去掉title中的<>标签
        size_t title_begin = _format_xml.find("<content>");
        size_t title_end   = _format_xml.find("</content>");
        size_t ignore_size = _format_xml.substr(title_end, _format_xml.size()).size();

        string title_tag = _format_xml.substr(title_begin, _format_xml.substr(title_begin).size() - ignore_size + 10);

        string process_title = title_tag.substr(title_tag.find("<content>") + 9, title_tag.size() - 19);

        size_t left_arrow = process_title.find("<");
        size_t right_arrow = process_title.find(">");

        if(left_arrow == string::npos || right_arrow == string::npos)
        {
            // std::cerr << "\t[ERROR] : Error Page -> : " << process_title << "\n";
            return;
        }

        _format_xml.erase(title_begin + left_arrow + 9, 1);
        _format_xml.erase(title_begin + right_arrow + 8, 1);

        doc.Parse(_format_xml.c_str());
        page_elem = doc.FirstChildElement("webpage");
    }
    

    //if(page_elem)
    //{
        XMLElement * docid_elem   = page_elem->FirstChildElement("docid");
        XMLElement * title_elem   = page_elem->FirstChildElement("title");
        XMLElement * link_elem    = page_elem->FirstChildElement("link");
        XMLElement * content_elem = page_elem->FirstChildElement("content");

        _id      = std::stoi(docid_elem->GetText());
        _title   = title_elem->GetText();
        _url     = link_elem->GetText();
        _content = content_elem->GetText();
    //}
}

void WebPage::generateWordFrequenceMap(SplitTool * p_split_tool)
{
    string page_content(_content);

    extractChineseWord(page_content);

    vector<string> split_res = p_split_tool->cut(page_content);

    for(auto & word : split_res)
    {
        if(!isStopWord(word))
        {
            if(word.empty())continue;

            _word_frequence[word]++;
        }
    }
}

void WebPage::generateFigurePrint(Simhasher * p_simhasher)
{
    p_simhasher->make(_content, 5, _figure_print);
}

bool WebPage::isStopWord(string & word)
{
    auto stop_set = Configuration::getInstance()->getStopWordSet();

    return stop_set.find(word) != stop_set.end();
}

void WebPage::extractChineseWord(string & word)
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

bool WebPage::isChinese(char ch)
{
    return 0x80 & ch;
}

size_t WebPage::nBytesCode(char ch)
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

// Test Unit
// int main(void)
// {
//     ifstream ifs("/home/lhc/My_Code/SearchEngine/test/webpage.dat");
//
//     ifs.seekg(2146);
//
//     char * buf = new char [9999]();
//     ifs.read(buf, 9999);
//
//     string s(buf);
//
//     SplitToolCppJieba jb;
//
//     string dictPath = Configuration::getInstance()->getConfig("DICT_PATH");
//     string modelPath = Configuration::getInstance()->getConfig("HMM_PATH");
//     string idfPath = Configuration::getInstance()->getConfig("IDF_PATH");
//     string stopWords = Configuration::getInstance()->getConfig("STOP_WORD_PATH");
//
//     Simhasher mhash(dictPath, modelPath, idfPath, stopWords);
//
//     WebPage wp(s);
//
//     wp.processWebPage(&jb, &mhash);
//
//     return 0;
// }
