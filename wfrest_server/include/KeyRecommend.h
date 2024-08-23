#ifndef __KEY_RECOMMEND_H__
#define __KEY_RECOMMEND_H__

#include "Dictionary.h"
#include "nlohmann/json.hpp"
#include <math.h>
#include <queue>

using std::priority_queue;
using STR_DISTANCE_FREQUENCE = pair<string, pair<int, int>>;

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

class KeyRecommend
{
public:
    KeyRecommend();

    void doRecommend(string query_word);

    string getResult();

private:
    void getRecommendWordSet(string query_word, set<int> & merge_set);
    void generateResult(string query_word, set<int> & merge_set);

private:
    int minEditDistance(string first_word, string second_word);

    size_t nBytesCode(const char ch);

    size_t length(const string & str);

    int triple_min(const int & lhs, const int & mhs, const int & rhs);

private:
    priority_queue<STR_DISTANCE_FREQUENCE> _pri_que;

    string _sought;
};

#endif
