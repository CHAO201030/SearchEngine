#ifndef __WEB_SEARCH_H__
#define __WEB_SEARCH_H__

#include "utilities.h"
#include "./nlohmann/json.hpp"
#include <tinyxml2.h>

struct candidatePage
{
    int doc_id;
    double similarity;
};

class WebPage
{
public:
    WebPage(string & fomat_xml);
    void extractWebPage();
    string getTitle();
    string getURL();
    string getContent();

private:
    string _page;
    string _title;
    string _url;
    string _content;
};

class WebSearch
{
public:
    string search(string query_word);
    string search(vector<int> doc_id_vec);

private:
    vector<string> getQueryVector(string query_word);
    set<int> getCandidateWebpageSet(vector<string> query_vec);
    set<int> extractDocId(unordered_map<int, double> um);
    unordered_map<int, vector<double>> getDocIdWeightMap(set<int> doc_id_set, vector<string> query_vec);
    vector<double> getQueryTFIDFVector(vector<string> query_word);
    void calcL2Norm(vector<double> & vec);
    vector<candidatePage> getCosSimilarityVec(vector<double> base_vec, unordered_map<int, vector<double>> doc_id_weight_map);
    priority_queue<candidatePage> generateResult(vector<candidatePage> cos_simi_vec);
    vector<vector<string>> generateWebPage(priority_queue<candidatePage> pq);


};

bool operator<(const candidatePage & lhs, const candidatePage & rhs);

#endif