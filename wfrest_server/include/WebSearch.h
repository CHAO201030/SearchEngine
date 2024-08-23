#ifndef __WEB_SEARCH_H__
#define __WEB_SEARCH_H__

#include "Dictionary.h"
#include "WebPage.h"
#include "nlohmann/json.hpp"

#include <math.h>
#include <queue>
using std::priority_queue;

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

class WebSearch
{
public:
    WebSearch();

    void doSearch(string query_word);

    string getResult();

private:
    void generateQueryWordVector(vector<string> & split_res, string query_word);
    void generateQueryWordFrequenceMap(map<string, int> & word_frequence_map, vector<string> split_res);
    void generateCandidateWebPageSet(set<int> & web_page_set, vector<string> split_res);

    void generateBaseVector(vector<double> & base_vector, map<string, int> & word_frequence_map, vector<string> split_res);
    void minMaxScaler(vector<double> & base_vector);
    void findCandidateWebPageID(map<int, vector<double>> & pages_feature_ves_map, set<int> & web_page_set, vector<string> split_res);

    void generateIdCosSimilarityVector(vector<pair<int, double>> & cos_similar_ves, map<int, vector<double>> & pages_feature_ves_map, vector<double> & base_vector);
    void generateCandidateDocIDVector(vector<int> & most_similar_doc_id_vec, vector<pair<int, double>> & cos_similar_ves);
    void generateWebPage(vector<int> & most_similar_doc_id_vec);

private:
    priority_queue<pair<int, double>> _pri_que;

    vector<vector<string>> _res_ves;

    SplitTool * _p_split_tool;

    bool _no_result = false;
};

#endif
