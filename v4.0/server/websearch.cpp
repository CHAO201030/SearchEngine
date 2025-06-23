#include "./include/websearch.h"

bool operator<(const candidatePage & lhs, const candidatePage & rhs)
{
    return lhs.similarity > rhs.similarity;
}

string WebSearch::search(string query_word)
{
    vector<string> query_vec = getQueryVector(query_word);
    set<int> doc_id_set = getCandidateWebpageSet(query_vec);

    if(doc_id_set.empty()) return {};

    unordered_map<int, vector<double>> doc_id_weight_map = getDocIdWeightMap(doc_id_set, query_vec);
    vector<double> base_vec = getQueryTFIDFVector(query_vec);
    vector<candidatePage> cos_simi_vec = getCosSimilarityVec(base_vec, doc_id_weight_map);
    priority_queue<candidatePage> pq = generateResult(cos_simi_vec);
    vector<vector<string>> page_vec = generateWebPage(pq);

    nlohmann::json ret;

    for(vector<string> & page : page_vec)
    {
        string title = "<p>" + page[0] + "</p>";
        string url   = "<a href='" + page[1] + "'>";
        string content = "<p>" + page[2] + "</p>";
        string res = "<div>" + url + title + "</a>" + content + "</div>";

        ret.push_back(res);
    }

    return ret.dump();
}

string WebSearch::search(vector<int> doc_id_vec)
{
    vector<vector<string>> page_vec;
    std::ifstream webpage_lib_ifs(Configuration::getInstance()->getConfig("webpage_lib"));
    auto offset_lib = Dictionary::getInstance()->getOffsetLib();

    for(int doc_id : doc_id_vec)
    {
        int offset = offset_lib[doc_id - 1].first;
        int length = offset_lib[doc_id - 1].second;

        webpage_lib_ifs.seekg(offset);

        char * buf = new char[length + 1]();

        webpage_lib_ifs.read(buf, length);

        string b(buf);

        WebPage page(b);

        page.extractWebPage();

        vector<string> temp = {page.getTitle(), page.getURL(), page.getContent()};

        page_vec.push_back(temp);

        delete [] buf;
    }

    webpage_lib_ifs.close();

    nlohmann::json ret;

    for(vector<string> & page : page_vec)
    {
        string title = "<p>" + page[0] + "</p>";
        string url   = "<a href='" + page[1] + "'>";
        string content = "<p>" + page[2] + "</p>";
        string res = "<div>" + url + title + "</a>" + content + "</div>";

        ret.push_back(res);
    }

    return ret.dump();
}

vector<string> WebSearch::getQueryVector(string query_word)
{
    auto stop_word_set = Configuration::getInstance()->getStopWordSet();
    SplitTool * split_tool = Dictionary::getInstance()->getSplitTool();
    vector<string> query_vec = split_tool->cut(query_word);

    for(auto p_str = query_vec.begin(); p_str != query_vec.end();)
    {
        if(stop_word_set.find(*p_str) != stop_word_set.end())
        {
            p_str = query_vec.erase(p_str);
        }
        else
        {
            ++p_str;
        }
    }

    return query_vec;
}

set<int> WebSearch::getCandidateWebpageSet(vector<string> query_vec)
{
    auto invert_idx_map = Dictionary::getInstance()->getInvertIndexLib();
    vector<set<int>> all_set_vec;

    // 获取包含每个query的文章ID集合
    for(string word : query_vec)
    {
        auto iim_it = invert_idx_map.find(word);
        if(iim_it != invert_idx_map.end())
        {
            all_set_vec.push_back(extractDocId(iim_it->second));
        }
    }

    if(all_set_vec.empty()) return {};

    // 取交集
    set<int> doc_id_set = *all_set_vec.begin();
    for(auto it = all_set_vec.begin() + 1; it != all_set_vec.end(); ++it)
    {
        set<int> temp;

        std::set_intersection(
            doc_id_set.begin(),
            doc_id_set.end(),
            it->begin(),
            it->end(),
            std::inserter(temp, temp.begin())
        );
        
        std::swap(doc_id_set, temp);
    }

    return doc_id_set;
}

set<int> WebSearch::extractDocId(unordered_map<int, double> um)
{
    set<int> ret;

    for(auto it : um)
    {
        ret.insert(it.first);
    }

    return ret;
}

vector<double> WebSearch::getQueryTFIDFVector(vector<string> query_word)
{
    vector<double> ret;
    unordered_map<string, int> word_freq_map;   // TF

    for(string str : query_word)
    {
        word_freq_map[str]++;
    }

    for(string str : query_word)
    {
        int N  = 1;
        int DF = 1;
        int TF = word_freq_map[str];
        double temp = double(N)/(DF + 1) + double(1);
        double IDF = log2(temp);

        ret.push_back(TF * IDF);
    }

    calcL2Norm(ret);

    return ret;
}

void WebSearch::calcL2Norm(vector<double> & vec)
{
    double norm = 0;

    for(double w : vec)
    {
        norm += w * w;
    }

    norm = sqrt(norm);

    for(double & w : vec)
    {
        w = w / norm;
    }
}

unordered_map<int, vector<double>> WebSearch::getDocIdWeightMap(set<int> doc_id_set, vector<string> query_vec)
{
    auto invert_idx_map = Dictionary::getInstance()->getInvertIndexLib();
    unordered_map<int, vector<double>> ret;

    for(int doc_id : doc_id_set)
    {
        vector<double> temp;    // w1_w, w2_w ,... wn_w

        for(string word : query_vec)
        {
            auto id_weight_map = invert_idx_map[word];  // word : {doc_id, weight}
            temp.push_back(id_weight_map[doc_id]);
        }

        ret[doc_id] = temp;
    }

    return ret;
}

vector<candidatePage> WebSearch::getCosSimilarityVec(vector<double> base_vec, unordered_map<int, vector<double>> doc_id_weight_map)
{
    vector<candidatePage> ret;
    double base_norm = 0;

    for(double w : base_vec)
    {
        base_norm += w * w;
    }

    base_norm = sqrt(base_norm);

    for(auto uit : doc_id_weight_map)
    {
        candidatePage page;

        page.doc_id = uit.first;

        double page_norm = 0;
        double inner_product = 0;
        vector<double> page_vec = uit.second;
        
        for(size_t i = 0; i < page_vec.size(); i++)
        {
            page_norm += page_vec[i] * page_vec[i];
            inner_product += base_vec[i] * page_vec[i];
        }

        page_norm = sqrt(page_norm);

        page.similarity = page_vec.size() != 1 ? inner_product / (base_norm * page_norm) : page_vec[0];
        
        ret.push_back(page);
    }

    return ret;
}

priority_queue<candidatePage> WebSearch::generateResult(vector<candidatePage> cos_simi_vec)
{
    priority_queue<candidatePage> pq;

    for(candidatePage page : cos_simi_vec)
    {
        if(pq.size() < 10)
        {
            pq.push(page);
        }
        else
        {
            if(page.similarity > pq.top().similarity)
            {
                pq.pop();
                pq.push(page);
            }
        }
    }

    return pq;
}

vector<vector<string>> WebSearch::generateWebPage(priority_queue<candidatePage> pq)
{
    vector<vector<string>> ret;

    vector<int> doc_id_vec(pq.size());
    for(int i = pq.size() - 1; i >= 0; i--)
    {
        doc_id_vec[i] = pq.top().doc_id;
        pq.pop();
    }

    std::ifstream webpage_lib_ifs(Configuration::getInstance()->getConfig("webpage_lib"));
    auto offset_lib = Dictionary::getInstance()->getOffsetLib();

    for(int doc_id : doc_id_vec)
    {
        int offset = offset_lib[doc_id - 1].first;
        int length = offset_lib[doc_id - 1].second;

        webpage_lib_ifs.seekg(offset);

        char * buf = new char[length + 1]();

        webpage_lib_ifs.read(buf, length);

        string b(buf);

        WebPage page(b);

        page.extractWebPage();

        vector<string> temp = {page.getTitle(), page.getURL(), page.getContent()};

        ret.push_back(temp);

        delete [] buf;
    }

    webpage_lib_ifs.close();

    return ret;
}



WebPage::WebPage(string & format_xml):_page(format_xml)
{

}

void WebPage::extractWebPage()
{
    using namespace tinyxml2;
    XMLDocument doc;
    doc.Parse(_page.c_str());

    XMLElement * page_elem = doc.FirstChildElement("webpage");

    if(page_elem)
    {
        XMLElement * title_elem   = page_elem->FirstChildElement("title");
        XMLElement * link_elem    = page_elem->FirstChildElement("link");
        XMLElement * content_elem = page_elem->FirstChildElement("content");

        _title   = title_elem->GetText();
        _url     = link_elem->GetText();
        _content = content_elem->GetText();
    }
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