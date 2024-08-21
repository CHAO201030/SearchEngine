#include "../include/NetServer.h"

template<>
struct std::less<STR_DISTANCE_FREQUENCE>
{
    bool operator()(const STR_DISTANCE_FREQUENCE & lhs, const STR_DISTANCE_FREQUENCE & rhs)
    {
        if(lhs.second.first < rhs.second.first)
        {
            return true;
        }
        else if(lhs.second.first == rhs.second.first)
        {
            if(lhs.second.second < rhs.second.second)
            {
                return true;
            }
        }

        return false;
    }
};

template<>
struct std::less<pair<int, double>>
{
    bool operator()(const pair<int, double> & lhs, const pair<int, double> & rhs)
    {
        if(lhs.second < rhs.second)
        {
            return false;
        }

        return true;
    }
};

template<class T>
std::ostream & operator<<(std::ostream & os, set<T> & rhs)
{
    for(auto & str : rhs)
    {
        os << str << " ";
    }

    return os;
}

NetServer::NetServer(int count):_wait_group(count)
{
    _p_split_tool = new SplitToolCppJieba;
}

NetServer::~NetServer()
{

}

void NetServer::start()
{
    int port = atoi(Configuration::getInstance()->getConfig("PORT").c_str());
    if(_server.track().start(port) == 0)
    {
        _server.list_routes();
        _wait_group.wait();
        _server.stop();
    }
    else
    {
        cerr << "[ERROR] : HTTP Server Satrt Failed...\n";
        exit(-1);
    }
}

void NetServer::loadModules()
{
    loadDataSet();
    loadStaticResourceMoudle();

    keyWordRecommendMoudle();
    webPageSearchMoudle();
}

void NetServer::loadDataSet()
{
    loadDictFile();
    loadWebPageFile();
}

void NetServer::loadDictFile()
{
    ifstream en_dict_ifs(Configuration::getInstance()->getConfig("english_dict.dat"));
    ifstream zh_dict_ifs(Configuration::getInstance()->getConfig("chinese_dict.dat"));

    ifstream en_index_ifs(Configuration::getInstance()->getConfig("english_dict_index.dat"));
    ifstream zh_index_ifs(Configuration::getInstance()->getConfig("chinese_dict_index.dat"));

    string line;

    // 1. 加载英文字典
    while(getline(en_dict_ifs, line))
    {
        istringstream iss(line);
        
        string word;
        
        int frequence;

        iss >> word >> frequence;
    
        _dict_freq_vec.emplace_back(word, frequence);
    }

    // 字典偏移英文单词数
    int en_offset = _dict_freq_vec.size();

    // 2. 加载中文字典
    while(getline(zh_dict_ifs, line))
    {
        istringstream iss(line);
        
        string word;
        
        int frequence;

        iss >> word >> frequence;
    
        _dict_freq_vec.emplace_back(word, frequence);
    
    }

    // 3. 加载英文索引
    while(getline(en_index_ifs, line))
    {
        istringstream iss(line);

        string word;
        
        int index;

        iss >> word;

        while(iss.good())
        {
            iss >> index;

            _word_index_map[word].insert(index);
        }
    }

    // 4. 加载中文索引
    while(getline(zh_index_ifs, line))
    {
        istringstream iss(line);

        string word;
        
        int index;

        iss >> word;

        while(iss.good())
        {
            iss >> index;

            _word_index_map[word].insert(index + en_offset);
        }
    }
}

void NetServer::loadWebPageFile()
{
    ifstream unrepeated_offset_lib_ifs(Configuration::getInstance()->getConfig("unrepeated_offsetlib.dat"));
    ifstream invert_index_lib_ifs(Configuration::getInstance()->getConfig("invert_index_lib.dat"));

    string line;
    while(getline(unrepeated_offset_lib_ifs, line))
    {
        istringstream iss(line);
        
        int doc_id = 0, offset = 0, page_len = 0;

        iss >> doc_id >> offset >> page_len;

        _offset_lib_ves.emplace_back(offset, page_len);
    }

    while(getline(invert_index_lib_ifs, line))
    {
        istringstream iss(line);

        string word;

        int doc_id = 0;

        double weight = 0;

        iss >> word;

        while(!iss.eof())
        {
            iss >> doc_id >> weight;

            _invert_index_lib[word][doc_id] = weight;
        }
    }
}

void NetServer::loadStaticResourceMoudle()
{
    _server.GET("/", 
    
    [](const HttpReq * req, HttpResp * resp)
    {
        // resp->File("./static/homepage.html");
        resp->File("static/1.html");
        // resp->File("static/favicon.ico");
    }
    );
}

void NetServer::keyWordRecommendMoudle()
{
    _server.GET("/recommend", 
    
    [this](const HttpReq * req, HttpResp * resp, SeriesWork * series)
    {
        string encode_uri = req->query("query");
        string query_word = uriDecode(encode_uri);
        if(query_word.empty())
        {
            resp->String("No query result");
        }
        else
        {
            nlohmann::json res_json;
            vector<string> recommand_word_vec = recommendHelper(query_word);

            for(size_t i = 0; i < recommand_word_vec.size(); i++)
            {
                res_json.push_back(recommand_word_vec[i]);  
            }

            resp->String(res_json.dump());
        }
    });
}

void NetServer::webPageSearchMoudle()
{

    _server.GET("/search",
    
    [this](const HttpReq * req, HttpResp * resp, SeriesWork * series)
    {
        string encode_uri = req->query("query");
        string query_word = uriDecode(encode_uri);

        if(query_word.empty())
        {
            resp->String("No query result");
        }
        else
        {
            nlohmann::json res_json;
            vector<vector<string>> recommand_word_vec = searchWebPageHelper(query_word);

            for(size_t i = 0; i < recommand_word_vec.size(); i++)
            {
                res_json.push_back(*recommand_word_vec[i].begin());
            }

            

            resp->String(res_json.dump());
        }
    });
}

vector<vector<string>> NetServer::searchWebPageHelper(string query_word)
{
    ifstream web_page_lib_ifs(Configuration::getInstance()->getConfig("unrepeated_webpage.dat"));

    unordered_set<string> & stop_word_set = Configuration::getInstance()->getStopWordSet();

    vector<string> split_res = _p_split_tool->cut(query_word);

    auto it = split_res.begin();

    while(it != split_res.end())
    {
        if(stop_word_set.find(*it) != stop_word_set.end())
        {
            it = split_res.erase(it);
            continue;
        }
        ++it;
    }

    map<string, int> word_frequence_map;

    for(size_t i = 0; i < split_res.size(); i++)
    {
        word_frequence_map[split_res[i]]++;
    }

    vector<set<int>> set_ves;

    for(size_t i = 0; i < split_res.size(); i++)
    {
        auto invert_idx_mit = _invert_index_lib.find(split_res[i]);

        set<int> temp;

        if(invert_idx_mit != _invert_index_lib.end())
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

    if(set_ves.size() == 0) return {{"No query result"}};

    set<int> web_page_set = queryWordSetIntersection(set_ves);

    if(web_page_set.size() == 0) return {{"No query result"}};

    vector<double> base_vector = calcQueryWordTFIDF(split_res, word_frequence_map);

    // TODO : 现在有了 查询词向量、查询词权重向量、包含查询词的网页交集集合
    //        要根据查询词向量里的词去找倒排索引表中 id为doc_id的权重值 将这些权重值组成网页的查询词向量然后算相似度

    // 1. 找包含所有查询词的文章id
    map<int, vector<double>> pages_feature_ves_map;

    for(auto doc_id : web_page_set)
    {
        // 1.1 当前文章的特征向量
        vector<double> cur_page_feature_vec;

        // 1.2 遍历query词
        for(size_t i = 0; i < split_res.size(); i++)
        {
            // 1.3 在倒排索引中找到这个单词(目测是一定能找到)

            auto word_id_feature_umap = _invert_index_lib.find(split_res[i]);

            if(word_id_feature_umap != _invert_index_lib.end())
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

    // 2. 计算余弦相似度
    vector<pair<int, double>> cos_similar_ves = calcCosSimilarity(pages_feature_ves_map, base_vector);

    // 3. 生成优先级队列
    priority_queue<pair<int, double>> candidate_web_page_queue;

    for(size_t i = 0; i < cos_similar_ves.size(); i++)
    {
        if(candidate_web_page_queue.size() >= 10)
        {
            if(cos_similar_ves[i].second > candidate_web_page_queue.top().second)
            {
                candidate_web_page_queue.pop();
                candidate_web_page_queue.push(cos_similar_ves[i]);
            }
        }
        else
        {
            candidate_web_page_queue.push(cos_similar_ves[i]);
        }
    }

    vector<int> most_similar_doc_id_vec(candidate_web_page_queue.size());

    for(int i = candidate_web_page_queue.size(); i > 0; i--)
    {
        most_similar_doc_id_vec[i - 1] = candidate_web_page_queue.top().first;
        candidate_web_page_queue.pop();
    }

    // 4. 获取doc_id对应的文章
    vector<vector<string>> web_page_ret_ves;
    size_t web_page_count = most_similar_doc_id_vec.size();

    for(size_t i = 0; i < web_page_count; i++)
    {
        int doc_id = most_similar_doc_id_vec[i];

        int offset = _offset_lib_ves[doc_id - 1].first;
        int page_len = _offset_lib_ves[doc_id - 1].second;

        web_page_lib_ifs.seekg(offset);

        char * p_buf = new char[page_len + 1]();

        web_page_lib_ifs.read(p_buf, page_len);

        string content = p_buf;
        vector<string> cur_page;
        cur_page.emplace_back(content);
        web_page_ret_ves.emplace_back(cur_page);

        delete [] p_buf;
    }

    // 5. 拼成string返回
    return web_page_ret_ves;
}

vector<pair<int, double>> NetServer::calcCosSimilarity(map<int, vector<double>> & pages_feature_ves_map, vector<double> & base_vector)
{
    vector<pair<int, double>> ret;

    if(pages_feature_ves_map.begin()->second.size() != base_vector.size())
    {
        cerr << "calcCosSimilarity dimension not equal : base_vector_dim = " << base_vector.size() << ", page_vector = " << pages_feature_ves_map.size() << "\n";
        return  {};
    }

    size_t dimension = base_vector.size();

    // 1. 计算基准向量的模
    double base_vec_norm = 0;

    for(size_t i = 0; i < dimension; i++)
    {
        base_vec_norm += (base_vector[i] * base_vector[i]);
    }

    base_vec_norm = sqrt(base_vec_norm);

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
        
        ret.emplace_back(page_feature_vec_mit->first, similarity);

        ++page_feature_vec_mit;
    }


    return ret;
}

set<int> NetServer::queryWordSetIntersection(vector<set<int>> & set_ves)
{
    set<int> ret = set_ves[0];

    for(size_t i = 1; i < set_ves.size(); i++)
    {
        // 迭代计算 保存上一次的结果

        vector<int> temp;

        set_intersection(ret.begin(), ret.end(), (set_ves[i]).begin(), (set_ves[i]).end(), back_inserter(temp));

        ret = set<int>(temp.begin(), temp.end());
    }

    return ret;
}

vector<double> NetServer::calcQueryWordTFIDF(vector<string> split_res, map<string, int> & word_frequence_map)
{
    vector<double> res;

    size_t N  = 1;

    for(size_t i = 0;i < split_res.size(); i++)
    {
        string word = split_res[i];
        size_t TF = word_frequence_map[word];
        size_t DF = 1;

        double temp = double(N)/(DF + 1) + double(1);   // 1 for IDF is positive number  eg, log2(1) = 0

        double IDF = log2(temp);

        double w = TF * IDF;

        res.emplace_back(w);
    }

    minMaxScaler(res);

    return res;
}

void NetServer::minMaxScaler(vector<double> & base_vector)
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

vector<string> NetServer::recommendHelper(string query_word)
{
    return generateCandidateWordSet(query_word);
}

string NetServer::uriDecode(const string & encodedURI)
{
    std::ostringstream decoded;
    size_t len = encodedURI.length();

    for (size_t i = 0; i < len; ++i)
    {
        if (encodedURI[i] == '%' && i + 2 < len)
        {
            std::string hex = encodedURI.substr(i + 1, 2);
            char decodedChar = static_cast<char>(std::stoi(hex, nullptr, 16));
            decoded << decodedChar;
            i += 2;
        }
        else if (encodedURI[i] == '+')
        {
            decoded << ' ';
        }
        else
        {
            decoded << encodedURI[i];
        }
    }

    return decoded.str();
}

vector<string> NetServer::generateCandidateWordSet(string query_word)
{
    set<string> unrepeated_character;
/*
    vector<string> en_query_word, zh_query_word;
    string en_word, zh_word;

    for(size_t i = 0; i < query_word.size();)
    {
        size_t nbyte = nBytesCode(query_word[i]);
        if(query_word[i] & 0x80 && nbyte != 1)
        {
            // 中文
            if(!en_word.empty())
            {
                en_query_word.push_back(en_word);
                en_word.clear();
            }
            zh_word += query_word.substr(i, nbyte);
        }
        else
        {
            // 英文
            if(!zh_word.empty())
            {
                zh_query_word.push_back(zh_word);
                zh_word.clear();
            }
            en_word += query_word[i];
        }
        
        i += nbyte;
    }

    if(!en_word.empty())
    {
        en_query_word.push_back(en_word);
        en_word.clear();
    }

    if(!zh_word.empty())
    {
        zh_query_word.push_back(zh_word);
        zh_word.clear();
    }

    for(size_t i = 0; i < en_query_word.size(); i++)
    {
        for(size_t j = 0; j < en_query_word[i].size();)
        {
            size_t  nbytes = nBytesCode(en_query_word[i][j]);
            en_query_word[i][j] = tolower(en_query_word[i][j]);
            unrepeated_character.insert(en_query_word[i].substr(j, nbytes));
            j += nbytes;
        }
    }

    for(size_t i = 0; i < zh_query_word.size(); i++)
    {
        for(size_t j = 0; j < zh_query_word[i].size();)
        {
            size_t  nbytes = nBytesCode(zh_query_word[i][j]);
            zh_query_word[i][j] = tolower(zh_query_word[i][j]);
            unrepeated_character.insert(zh_query_word[i].substr(j, nbytes));
            j += nbytes;
        }
    }
*/
    
    // 找集合
    for(size_t i = 0; i < query_word.size();)
    {
        size_t nbytes = nBytesCode(query_word[i]);

        unrepeated_character.insert(query_word.substr(i, nbytes));

        i += nbytes;
    }

    vector<set<int> * > ch_set_vec;

    for(string ch : unrepeated_character)
    {
        auto index_map_it = _word_index_map.find(ch);
        if(index_map_it != _word_index_map.end())
        {
            ch_set_vec.push_back(&index_map_it->second);
        }
    }

    // 取并集
    set<int> merge_set;
    for(size_t i = 0; i < ch_set_vec.size(); i++)
    {
        set<int> * p_qword_set = ch_set_vec[i];

        auto sit = p_qword_set->begin();
        while (sit != p_qword_set->end())
        {
            merge_set.insert(*sit);
            ++sit;
        }
    }
    // cout << "Query character : " << unrepeated_character << "\n";
    // cout << "Result set size = " << merge_set.size() << "\n";
    // 返回推荐词
    priority_queue<STR_DISTANCE_FREQUENCE> recommend_word_queue;

    auto sit = merge_set.begin();
    while(sit != merge_set.end())
    {
        STR_DISTANCE_FREQUENCE word_d_f;
    
        if(query_word.size() * 2 >= _dict_freq_vec[*sit].first.size())
        {
            word_d_f.first = _dict_freq_vec[*sit].first;

            word_d_f.second.first = minEditDistance(query_word, _dict_freq_vec[*sit].first);

            word_d_f.second.second = _dict_freq_vec[*sit].second;

            if(recommend_word_queue.size() >= 10)
            {
                if(word_d_f.second.first < recommend_word_queue.top().second.first)
                {
                    recommend_word_queue.pop();
                    recommend_word_queue.push(word_d_f);
                }
            }
            else
            {
                recommend_word_queue.push(word_d_f);
            }
        }
        ++sit;
    }

    vector<string> recommend_words_ves(recommend_word_queue.size());

    for(int i = recommend_word_queue.size() - 1; i >= 0; i--)
    {
        recommend_words_ves[i] = recommend_word_queue.top().first;
        recommend_word_queue.pop();
    }

    return recommend_words_ves;
}

int NetServer::minEditDistance(string first_word, string second_word)
{
// 1. 得到中文字符串人类视角的长度
    size_t first_word_len = length(first_word);
    size_t second_word_len = length(second_word);

    // 2. 初始化 动态规划辅助数组
    int dp[first_word_len + 1][second_word_len + 1];

    for(size_t i = 0; i < first_word_len + 1; i++)
    {
        dp[i][0] = i;
    }
    for(size_t j = 0; j < second_word_len + 1; j++)
    {
        dp[0][j] = j;
    }

    // 3. 计算
    string first_sub_str, second_sub_str;

    for(size_t i = 1, first_idx = 0; i <= first_word_len; i++)
    {
        // 3.1 得到first_word的第一个人类视角字符及其所占字节数
        size_t bytes_num = nBytesCode(first_word[first_idx]);
        first_sub_str = first_word.substr(first_idx, bytes_num);
        first_idx += bytes_num;

        for(size_t j = 1, second_idx = 0; j <= second_word_len; j++)
        {
            // 3.2 得到second_word的第一个人类视角字符及其所占字节数
            bytes_num = nBytesCode(second_word[second_idx]);
            second_sub_str = second_word.substr(second_idx, bytes_num);
            second_idx += bytes_num;

            if(first_sub_str == second_sub_str)
            {
                // 4.1 如果字符串一样 不需要编辑 则编辑距离为上一次的编辑距离
                dp[i][j] = dp[i - 1][j - 1];
            }
            else
            {
                // 4.2 左 左上 上 最小的编辑距离
                dp[i][j] = triple_min(dp[i][j - 1] + 1, dp[i -1][j] + 1, dp[i - 1][j - 1] + 1);
            }
        }
    }

    // for(size_t i = 0; i < first_word_len + 1; i++)
    // {
    //     for(size_t j = 0; j < second_word_len + 1; j++)
    //     {
    //         cout << dp[i][j] << " ";
    //     }
    //     cout << "\n";
    // }

    return dp[first_word_len][second_word_len];
}

size_t NetServer::nBytesCode(const char ch)
{
    if(ch & (1 << 7))
    {
        int nBytes = 1;
        for(int idx = 0; idx != 6; ++idx)
        {
            if(ch & (1 << (6 - idx)))
            {
                ++nBytes;
            }
            else
            {
                break;
            }
        }
        return nBytes;
    }
    return 1;
}

size_t NetServer::length(const string & str)
{
    size_t ilen = 0;

    for(size_t idx = 0; idx != str.size(); ++idx)
    {
        int nBytes = nBytesCode(str[idx]);
        idx += (nBytes - 1);
        ++ilen;
    }

    return ilen;
}

int NetServer::triple_min(const int & lhs, const int & mhs, const int & rhs)
{
    return lhs < mhs ? (lhs < rhs ? lhs : rhs) : (mhs < rhs ? mhs : rhs);
}
