#ifndef __RECOMMEND_H__
#define __RECOMMEND_H__

#include "utilities.h"
#include "./nlohmann/json.hpp"

struct candidateWord
{
    string word;
    int distance;
    int hot;
};

class KeyRecommend
{
public:
    string recommend(string query_word);

private:
    set<int> getCandidateWordSet(string query_word);
    priority_queue<candidateWord> genereateResult(string query_word, set<int> & candidate_set);
    size_t nByteCode(const char ch);
    size_t length(const string & str);
    int triple_min(const int & lhs, const int &mhs, const int & rhs);
    int minEditDistance(string first_word, string second_word);
};

bool operator<(const candidateWord & lhs, const candidateWord &rhs);

#endif