#include <xapian.h>
#include <cpp58.hpp>
#include <tinyxml2.h>
#include "../include/simhash/cppjieba/Jieba.hpp"


#define DICT_PATH "/home/lhc/My_Code/SearchEngine/data/dict/jieba.dict.utf8"
#define HMM_PATH "/home/lhc/My_Code/SearchEngine/data/dict/hmm_model.utf8"
#define USER_DICT_PATH "/home/lhc/My_Code/SearchEngine/data/dict/user.dict.utf8"
#define IDF_PATH "/home/lhc/My_Code/SearchEngine/data/dict/idf.utf8"
#define STOP_WORD_PATH "/home/lhc/My_Code/SearchEngine/data/dict/stop_words.utf8"

using namespace Xapian;

using namespace tinyxml2;

unordered_map<int, pair<int, int>> _offset_lib;

cppjieba::Jieba * _jieba = new cppjieba::Jieba(DICT_PATH, HMM_PATH, USER_DICT_PATH, IDF_PATH, STOP_WORD_PATH);

struct WebPage
{
    int _id;
    string _title;
    string _url;
    string _content;
};

WebPage extractWebPage(string _format_xml)
{
    XMLDocument doc;
    doc.Parse(_format_xml.c_str());
    
    XMLElement * page_elem    = doc.FirstChildElement("webpage");

    /* 这段 if 代码对于Xapian版本应该是没啥用了 但是我怕出bug 不敢动 */

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
            return {-1, "", "", ""};
        }

        _format_xml.erase(title_begin + left_arrow + 9, 1);
        _format_xml.erase(title_begin + right_arrow + 8, 1);

        doc.Parse(_format_xml.c_str());
        page_elem = doc.FirstChildElement("webpage");
    }

    XMLElement *docid_elem = page_elem->FirstChildElement("docid");
    XMLElement *title_elem = page_elem->FirstChildElement("title");
    XMLElement *link_elem = page_elem->FirstChildElement("link");
    XMLElement *content_elem = page_elem->FirstChildElement("content");

    int _id = std::stoi(docid_elem->GetText());
    string _title = title_elem->GetText();
    string _url = link_elem->GetText();
    string _content = content_elem->GetText();

    return {_id, _title, _url, _content};
}

void loadOffsetLib()
{
    ifstream offset_lib_ifs("../data/unrepeated_offsetlib.dat");

    string line;
    while(getline(offset_lib_ifs, line))
    {
        istringstream iss(line);

        int doc_id = 0, offset = 0, page_len = 0;

        iss >> doc_id >> offset >> page_len;

        _offset_lib.insert({doc_id, {offset, page_len}});
    }
}

void storeIntoDataBase(WritableDatabase & write_db, string content, WebPage cur_page)
{
    TermGenerator indexer;

    Document cur_doc;

    cur_doc.add_value(0, to_string(cur_page._id));
    cur_doc.add_value(1, cur_page._title);
    cur_doc.add_value(2, cur_page._url);

    cur_doc.set_data(cur_page._content);

    indexer.set_document(cur_doc);
    indexer.index_text(content);

    write_db.add_document(cur_doc);
    write_db.commit();
}

void loadWebPageLib()
{
    WritableDatabase write_db("./xapian_db", DB_CREATE_OR_OVERWRITE);

    ifstream web_page_lib_ifs("../data/unrepeated_webpage.dat");

    for(size_t i = 0; i < _offset_lib.size(); i++)
    {
        double percentage = double(100) * i / _offset_lib.size();        
        printf("\t-> %5.2f%% : %ld\r", percentage, i);
        fflush(stdout);

        int doc_id = i + 1;
        int offset = _offset_lib[doc_id].first;
        int page_len = _offset_lib[doc_id].second;

        char * p_buf = new char [page_len + 1] ();

        web_page_lib_ifs.seekg(offset);

        web_page_lib_ifs.read(p_buf, page_len);

        string cur_page_content(p_buf);

        WebPage cur_page = extractWebPage(cur_page_content);

        vector<string> split_res;
        _jieba->Cut(cur_page._content, split_res);

        string to_xapian_str;

        for(size_t j = 0; j < split_res.size(); j++)
        {
            to_xapian_str += split_res[j];
            to_xapian_str += " ";
        }

        storeIntoDataBase(write_db, to_xapian_str, cur_page);
    }

    cout << "\t-> 100.00% : " << _offset_lib.size() << "\n";
}


int main()
{
    loadOffsetLib();
    loadWebPageLib();

    return 0;
}
