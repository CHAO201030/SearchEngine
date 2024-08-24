#include "../include/WebSearch.h"
#include <xapian.h>

using Xapian::WritableDatabase;
using Xapian::Enquire;
using Xapian::Query;
using Xapian::Document;
using Xapian::MSet;
using Xapian::MSetIterator;

WebSearch::WebSearch()
{
    _p_split_tool = Dictionary::getInstance()->getSpliTool();
}

void WebSearch::doSearch(string query_word)
{
    WritableDatabase data_base(Configuration::getInstance()->getConfig("xapian_data_base"), Xapian::DB_OPEN);

    Enquire enquire(data_base);

    vector<Query> query_vec;
    

    vector<string> split_res;
    generateQueryWordVector(split_res, query_word);

    for(size_t i = 0; i < split_res.size(); i++)
    {
        query_vec.emplace_back(split_res[i]);
    }

    Query real_query = Query(query_vec[0]);

    for(size_t i = 1; i < query_vec.size(); i++)
    {
        real_query = Query(Query::OP_AND, real_query, query_vec[i]);
    }

    std::cout << real_query.get_description() << "\n";

    enquire.set_query(real_query);

    MSet m = enquire.get_mset(0, 10);

    for(MSetIterator mit = m.begin(); mit != m.end(); ++mit)
    {
        Document cur_doc = mit.get_document();

        string id = cur_doc.get_value(0);       // 文章id
        string title = cur_doc.get_value(1);    // 文章标题
        string url = cur_doc.get_value(2);      // 文章链接
        string content = cur_doc.get_data();    // 文章内容

        vector<string> temp;
        temp.emplace_back(title);
        temp.emplace_back(url);
        temp.emplace_back(content);
    
        _res_ves.emplace_back(temp);
    }

    if(_res_ves.size() == 0) _no_result = true;
}

string WebSearch::getResult()
{
    if(_no_result)return{"No such web page"};

    nlohmann::json res_json;

    for(size_t i = 0; i < _res_ves.size(); i++)
    {
        string title = "<p>" + _res_ves[i][0] + "</p>";
        string link = "<a href='" + _res_ves[i][1] +"'>";
        string content = "<p>" + _res_ves[i][2] + "</p>";
        string res = "<div>" + link + title + "</a>" + content + "</div>";

        res_json.push_back(res);
    }

    return res_json.dump();
}

void WebSearch::generateQueryWordVector(vector<string> & split_res, string query_word)
{
    unordered_set<string> & stop_word_set = Configuration::getInstance()->getStopWordSet();

    split_res = _p_split_tool->cut(query_word);

    auto split_res_it = split_res.begin();

    while(split_res_it != split_res.end())
    {
        if(stop_word_set.find(*split_res_it) != stop_word_set.end())
        {
            split_res_it = split_res.erase(split_res_it);
            continue;
        }
        ++split_res_it;
    }
}

/********************************************************** Xapian 版本不再使用 ************************************************************/


void WebSearch::generateQueryWordFrequenceMap(map<string, int> & word_frequence_map, vector<string> split_res)
{
    for(size_t i = 0; i < split_res.size(); i++)
    {
        word_frequence_map[split_res[i]]++;
    }
}

void WebSearch::generateCandidateWebPageSet(set<int> & web_page_set, vector<string> split_res)
{
    vector<set<int>> set_ves;

    auto invert_index_lib = Dictionary::getInstance()->getInvertIndexLib();

    // 1. 查倒排索引找包含关键字的集合
    for(size_t i = 0; i < split_res.size(); i++)
    {
        auto invert_idx_mit = invert_index_lib.find(split_res[i]);

        set<int> temp;

        if(invert_idx_mit != invert_index_lib.end())
        {
            for(auto umit : invert_idx_mit->second)
            {
                temp.insert(umit.first);
            }
            set_ves.push_back(temp);
        }
        else
        {
            set_ves.push_back(temp);
        }
    }

    if(set_ves.empty())
    {
        _no_result = true;
        return;
    }

    // 2. 取交集
    web_page_set = set_ves[0];

    for(size_t i = 1; i < set_ves.size(); i++)
    {
        // 迭代计算 保存上一次的结果

        vector<int> temp;

        set_intersection(web_page_set.begin(), web_page_set.end(), (set_ves[i]).begin(), (set_ves[i]).end(), back_inserter(temp));

        web_page_set = set<int>(temp.begin(), temp.end());
    }

    if(web_page_set.empty())_no_result = true;
}

void WebSearch::generateBaseVector(vector<double> & base_vector, map<string, int> & word_frequence_map, vector<string> split_res)
{
    size_t N  = 1;

    for(size_t i = 0;i < split_res.size(); i++)
    {
        string word = split_res[i];
        size_t TF = word_frequence_map[word];
        size_t DF = 1;

        double temp = double(N)/(DF + 1) + double(1);   // 1 for IDF is positive number  eg, log2(1) = 0

        double IDF = log2(temp);

        double w = TF * IDF;

        base_vector.emplace_back(w);
    }

    minMaxScaler(base_vector);
}

void WebSearch::minMaxScaler(vector<double> & base_vector)
{
    double sigma_weight_squre = 0;

    for(size_t word_count = 0; word_count < base_vector.size(); word_count++)
    {
        double word_weight = base_vector[word_count];
        sigma_weight_squre += (word_weight * word_weight);
    }

    for(size_t word_count = 0; word_count < base_vector.size(); word_count++)
    {
        double word_weight = base_vector[word_count];

        base_vector[word_count] = word_weight / sqrt(sigma_weight_squre);
    }
}

void WebSearch::findCandidateWebPageID(map<int, vector<double>> & pages_feature_ves_map, set<int> & web_page_set, vector<string> split_res)
{
    auto invert_index_lib = Dictionary::getInstance()->getInvertIndexLib();

    for(auto doc_id : web_page_set)
    {
        // 1.1 当前文章的特征向量
        vector<double> cur_page_feature_vec;

        // 1.2 遍历query词
        for(size_t i = 0; i < split_res.size(); i++)
        {
            // 1.3 在倒排索引中找到这个单词(目测是一定能找到)
            auto word_id_feature_umap = invert_index_lib.find(split_res[i]);

            if(word_id_feature_umap != invert_index_lib.end())
            {
                // 1.4 找到这个单词后 找对应的 {doc_id, weight} 表(目测是一定能找到)
                auto id_feature_umap = word_id_feature_umap->second.find(doc_id);

                if(id_feature_umap != word_id_feature_umap->second.end())
                {
                    // 1.5 得到了 网页为 doc_id 的 word 的 weight
                    cur_page_feature_vec.push_back(id_feature_umap->second);
                }
            }
        }

        pages_feature_ves_map[doc_id] = cur_page_feature_vec;
    }
}

void WebSearch::generateIdCosSimilarityVector(vector<pair<int, double>> & cos_similar_ves, map<int, vector<double>> & pages_feature_ves_map, vector<double> & base_vector)
{
    if(pages_feature_ves_map.begin()->second.size() != base_vector.size())
    {
        std::cerr << "calcCosSimilarity dimension not equal : base_vector_dim = " << base_vector.size() << ", page_vector = " << pages_feature_ves_map.size() << "\n";
        return;
    }

    size_t dimension = base_vector.size();

    // 1. 计算基准向量的模
    double base_vec_norm = 0;

    for(size_t i = 0; i < dimension; i++)
    {
        base_vec_norm += (base_vector[i] * base_vector[i]);
    }

    base_vec_norm = sqrt(base_vec_norm);

    // 2. 算每一个文章向量的模
    auto page_feature_vec_mit = pages_feature_ves_map.begin();

    while(page_feature_vec_mit != pages_feature_ves_map.end())
    {
        vector<double> cur_page_feature_vec = page_feature_vec_mit->second;
        
        double page_vec_norm = 0;
        
        double inner_product = 0;
        
        for(size_t i = 0; i < dimension; i++)
        {
            page_vec_norm += (cur_page_feature_vec[i] * cur_page_feature_vec[i]);
            inner_product += (cur_page_feature_vec[i] * base_vector[i]);
        }
        
        page_vec_norm = sqrt(page_vec_norm);

        double similarity = inner_product / (base_vec_norm * page_vec_norm);
        
        cos_similar_ves.emplace_back(page_feature_vec_mit->first, similarity);

        ++page_feature_vec_mit;
    }

}

void WebSearch::generateCandidateDocIDVector(vector<int> & most_similar_doc_id_vec, vector<pair<int, double>> & cos_similar_ves)
{
    for(size_t i = 0; i < cos_similar_ves.size(); i++)
    {
        if(_pri_que.size() >= 10)
        {
            if(cos_similar_ves[i].second > _pri_que.top().second)
            {
                _pri_que.pop();
                _pri_que.push(cos_similar_ves[i]);
            }
        }
        else
        {
            _pri_que.push(cos_similar_ves[i]);
        }
    }

    most_similar_doc_id_vec.resize(_pri_que.size());

    for(int i = _pri_que.size() - 1; i >= 0; i--)
    {
        most_similar_doc_id_vec[i] = _pri_que.top().first;
        _pri_que.pop();
    }
}

void WebSearch::generateWebPage(vector<int> & most_similar_doc_id_vec)
{
    ifstream web_page_lib_ifs(Configuration::getInstance()->getConfig("unrepeated_webpage.dat"));

    auto offset_lib_ves = Dictionary::getInstance()->getOffsetLib();

    size_t web_page_count = most_similar_doc_id_vec.size();

    for(size_t i = 0; i < web_page_count; i++)
    {
        int doc_id = most_similar_doc_id_vec[i];

        int offset = offset_lib_ves[doc_id - 1].first;
        int page_len = offset_lib_ves[doc_id - 1].second;

        web_page_lib_ifs.seekg(offset);

        char * p_buf = new char[page_len + 1]();

        web_page_lib_ifs.read(p_buf, page_len);

        string cur_page_content = p_buf;

        WebPage cur_page(cur_page_content);

        cur_page.extractWebPage();

        vector<string> cur_page_vec = {cur_page.getTitle(), cur_page.getURL(), cur_page.getContent()};

        _res_ves.emplace_back(cur_page_vec);

        delete [] p_buf;
    }

    web_page_lib_ifs.close();
}

