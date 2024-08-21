#ifndef __NET_SERVER_H__
#define __NET_SERVER_H__

#include "Configuration.h"
#include "nlohmann/json.hpp"
#include "SplitToolCppJieba.h"
#include <workflow/WFFacilities.h>
#include <wfrest/HttpServer.h>
#include <iostream>
#include <vector>
#include <string>
#include <map>
#include <unordered_map>
#include <set>
#include <queue>
#include <utility>
#include <sys/time.h>
#include <sys/types.h>
#include <math.h>

using std::vector;
using std::pair;
using std::set;
using std::map;
using std::string;
using std::cout;
using std::cerr;
using std::priority_queue;
using std::unordered_map;
using namespace wfrest;

using STR_DISTANCE_FREQUENCE = pair<string, pair<int, int>>;

class NetServer
{
public:
    NetServer(int count);

    ~NetServer();

    void start();

    void loadModules();

private:
    void loadDataSet();
    void loadDictFile();
    void loadWebPageFile();
    void loadStaticResourceMoudle();

private:
    void keyWordRecommendMoudle();

    string uriDecode(const string & encode_str);

    vector<string> recommendHelper(string query_word);

    vector<string> generateCandidateWordSet(string query_word);

    int minEditDistance(string first_word, string second_word);

    size_t nBytesCode(const char ch);

    size_t length(const string & str);

    int triple_min(const int & lhs, const int & mhs, const int & rhs);

private:
    void webPageSearchMoudle();
    vector<vector<string>> searchWebPageHelper(string query_word);
    vector<double> calcQueryWordTFIDF(vector<string> split_res, map<string, int> & word_frequence_map);
    void minMaxScaler(vector<double> & base_vector);
    set<int> queryWordSetIntersection(vector<set<int>> & set_ves);
    vector<pair<int, double>> calcCosSimilarity(map<int, vector<double>> & pages_feature_ves_map, vector<double> & base_vector);
private:
    SplitTool * _p_split_tool = nullptr;    // 分词工具

    vector<pair<string, int>> _dict_freq_vec;   // 词频字典

    map<string, set<int>> _word_index_map;  // 词频字典索引表

    vector<pair<int, int>> _offset_lib_ves; // 去重网页偏移库

    unordered_map<string, unordered_map<int, double>> _invert_index_lib;    // 去重网页倒排索引库

    WFFacilities::WaitGroup _wait_group;    // 等待组

    wfrest::HttpServer _server; // workflow异步HTTP服务器
};

#endif
