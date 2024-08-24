#include <xapian.h>
#include <cpp58.hpp>
#include <tinyxml2.h>
#include "../include/simhash/cppjieba/Jieba.hpp"


#define DICT_PATH "/home/lhc/My_Code/SearchEngine/data/dict/jieba.dict.utf8"
#define HMM_PATH "/home/lhc/My_Code/SearchEngine/data/dict/hmm_model.utf8"
#define USER_DICT_PATH "/home/lhc/My_Code/SearchEngine/data/dict/user.dict.utf8"
#define IDF_PATH "/home/lhc/My_Code/SearchEngine/data/dict/idf.utf8"
#define STOP_WORD_PATH "/home/lhc/My_Code/SearchEngine/data/dict/stop_words.utf8"

/*
    提取网页生成索引 生成的索引需要采用Xapian的方式
*/

using namespace Xapian;
using namespace tinyxml2;

void test1()
{
    const std::string doc_id1 = "doc1";
    const std::string doc_title1 = "如何 构建 搜索引擎 搜索 引擎";
    const std::string doc_content1 = "how to build  search engine";
    const std::string doc_id2 = "doc2";
    const std::string doc_title2 = "搜索 是 一个 基本 技能";
    const std::string doc_content2 = "search is a basic skill";
 
    // 1. 创建了一个目录文件
    WritableDatabase write_db("./test_data", DB_CREATE_OR_OVERWRITE);
    TermGenerator indexer;  // 分割一段text

    Document doc1;
    doc1.add_value(101, doc_id1);
    doc1.set_data(doc_content1);

    Document doc2;
    doc2.add_value(101, doc_id2);
    doc2.set_data(doc_content2);

    indexer.set_document(doc1);
    indexer.index_text(doc_title1);

    indexer.set_document(doc2);
    indexer.index_text(doc_title2);


    write_db.add_document(doc1);
    write_db.add_document(doc2);

    write_db.commit();

    Enquire enquire(write_db);
    QueryParser qp;

    Query term1("搜索");
    Query term2("引擎");

    Query t1_or_t2 = Query(Query::OP_OR, term1, term2);
    
    // 打印类的信息

        /*  
            Query term 1   is : Query(搜索)
            Query term 2   is : Query(引擎)
            Query t1 or t2 is : Query((搜索 OR 引擎))
        */

    cout << "Query term 1   is : " << term1.get_description() << "\n";
    cout << "Query term 2   is : " << term2.get_description() << "\n";
    cout << "Query t1 or t2 is : " << t1_or_t2.get_description() << "\n";

    enquire.set_query(t1_or_t2);

    MSet matches = enquire.get_mset(0, 10);
    cout << matches.get_matches_estimated() << " result find\n";
    cout << "matches 1 -> " << matches.size() << "\n";

    for(MSetIterator msit = matches.begin(); msit != matches.end(); ++msit)
    {
        Document doc = msit.get_document();
        string doc_id = doc.get_value(101);
        cout << "Rank : " << msit.get_rank() + 1 << ", weight : " << msit.get_weight() << "\n";
        cout << "\t" << "doc_id = " << doc_id << "\n";
        cout << "\t" << "content\n\t\t" << doc.get_data() << "\n";
    }
}

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
    ifstream offset_lib_ifs("./data/offsetlib.dat");

    string line;
    while(getline(offset_lib_ifs, line))
    {
        istringstream iss(line);

        int doc_id = 0, offset = 0, page_len = 0;

        iss >> doc_id >> offset >> page_len;

        _offset_lib.insert({doc_id, {offset, page_len}});
    }
}

void generateDataBase(WritableDatabase & write_db, string content, WebPage cur_page)
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
    WritableDatabase write_db("./test2", DB_CREATE_OR_OPEN);
    
    ifstream web_page_lib_ifs("./data/webpagelib.dat");

    for(size_t i = 0; i < _offset_lib.size(); i++)
    {
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

        generateDataBase(write_db, to_xapian_str, cur_page);
    }
}



void test2()
{
    loadOffsetLib();
    loadWebPageLib();
}

int main()
{
    // test1();

    
    test2();

    return 0;
}