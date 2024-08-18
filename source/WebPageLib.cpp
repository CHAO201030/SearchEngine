#include "../include/WebPageLib.h"

void WebPageLib::create()
{
    string original_web_page_dir = Configuration::getInstance()->getConfig("original_web_page_dir");

    _dir_scanner.traverse(original_web_page_dir);

    vector<string> web_page_path = _dir_scanner.getFiles();

    cout << "[INFO] : Create Web Page Lib begin...\n";

    for(auto path : web_page_path)
    {
        processWebPage(path);
        cout << "\t-> " << path << " success\n";
    }

    cout << "[INFO] : Create Web Page Lib end...\n";


    cout << "[INFO] : Create Web Page Offset Lib begin...\n";

    int cur_page_length    = 0;
    int cur_page_begin_pos = 0;

    for(size_t i = 0; i < _web_pages.size(); i++)
    {
        cur_page_length = int(_web_pages[i].size());

        _offset_lib[i + 1] = {cur_page_begin_pos, cur_page_length - 1}; // -1 for \n

        cur_page_begin_pos += cur_page_length;
    }

    cout << "[INFO] : Create Web Page Offset Lib end...\n";
}

void WebPageLib::store()
{
    string web_page_lib_path = Configuration::getInstance()->getConfig("webpage.dat");
    string offset_lib_path = Configuration::getInstance()->getConfig("offsetlib.dat");

    ofstream ofs_web_page(web_page_lib_path);
    ofstream ofs_offset_lib(offset_lib_path);

    cout << "[INFO] : Store Web Page Lib begin...\n";
    cout << "\t-> " << web_page_lib_path << "\n";

    for(size_t i = 0; i < _web_pages.size(); i++)
    {
        ofs_web_page << _web_pages[i];
    }

    cout << "[INFO] : Store Web Page Lib end...\n";

    cout << "[INFO] : Store Web Page Offset Lib begin...\n";
    cout << "\t-> " << offset_lib_path << "\n";

    auto mit = _offset_lib.begin();
    while (mit != _offset_lib.end())
    {
        ofs_offset_lib << mit->first << " "
                       << mit->second.first << " " << mit->second.second
                       << "\n";

        ++mit;
    }

    cout << "[INFO] : Store Web Page Offset Lib end...\n";

    ofs_web_page.close();
    ofs_offset_lib.close();
}

void WebPageLib::processWebPage(const string file_path)
{
    XMLDocument doc;
    doc.LoadFile(file_path.c_str());

    XMLElement * item_element = doc.FirstChildElement("rss")
                                    ->FirstChildElement("channel")
                                    ->FirstChildElement("item");

    while(item_element)
    {
        string title, link, content;

        getRssItem(item_element, title, link, content);

        deleteHTMLTag(content);

        if(title != "NULL" && link != "NULL" && content != "NULL")
        {
            size_t error_title_begin_pos = title.find("&$<br>&$");

            if(error_title_begin_pos != std::string::npos)
            {
                title.erase(error_title_begin_pos, 8);
            }

            _web_pages.push_back(generateWebPage(title, link, content));
        }

        item_element = item_element->NextSiblingElement();
    }
}

void WebPageLib::getRssItem(XMLElement * item_element, string & title, string & link, string & content)
{
    XMLElement * p_title = item_element->FirstChildElement("title");
    XMLElement * p_link = item_element->FirstChildElement("link");
    XMLElement * p_content = item_element->FirstChildElement("description");

    if(p_content == nullptr)
    {
        p_content = item_element->FirstChildElement("content");
    }

    title = p_title ? p_title->GetText() : "NULL";
    
    link = p_link ? p_link->GetText() : "NULL";
    if(link.find("http") == std::string::npos)
    {
        link = "NULL";
    }

    content = p_content ? p_content->GetText() : "NULL";    
    if(content.size() == 0)
    {
        content = "NULL";
    }
}

void WebPageLib::deleteHTMLTag(string & content)
{
    regex html_tag_pattern("<[^>]*>");
    regex space_pattern(R"(^\s*)");

    content = std::regex_replace(content, html_tag_pattern, "");
    content = std::regex_replace(content, space_pattern, "");

    if(content.size() == 0) content = "NULL";
}

string WebPageLib::generateWebPage(string & title, string & link, string & content)
{
    ostringstream oss;

    oss << "<webpage>\n"
        << "\t<docid>"    << _web_pages.size() + 1 << "</docid>\n"
        << "\t<title>"    << title << "</title>\n"
        << "\t<link>"     << link  << "</link>\n"
        << "\t<content>"  << content << "</content>\n"
        << "</webpage>\n\0";

    return oss.str();
}
