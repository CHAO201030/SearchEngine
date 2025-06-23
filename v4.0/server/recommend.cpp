#include "./include/recommend.h"

bool operator<(const candidateWord & lhs, const candidateWord &rhs)
{
    if(lhs.distance < rhs.distance) return true;
    if(lhs.distance == rhs.distance && lhs.hot > rhs.hot) return true;
    return false;
}

string KeyRecommend::recommend(string query_word)
{
    set<int> candidate_set = getCandidateWordSet(query_word);
    priority_queue<candidateWord> pq = genereateResult(query_word, candidate_set);
    vector<string> recommend_word(pq.size());
    nlohmann::json ret;

    for(int i = pq.size() - 1; i >= 0; i--)
    {
        recommend_word[i] = pq.top().word;
        pq.pop();
    }

    for(string str : recommend_word)
    {
        ret.push_back(str);
    }

    return ret.dump();
}

set<int> KeyRecommend::getCandidateWordSet(string query_word)
{
    set<int> ret;
    set<string> unrepeated_character;
    auto _word_idx_map = Dictionary::getInstance()->getWordFreqIndexMap();  // {word, {dict_idx_1, dict_idx_2}}

    // split query_word ==> char_1, char_2, char_n
    for(size_t i = 0; i < query_word.size();)
    {
        size_t nbytes = nByteCode(query_word[i]);
        
        unrepeated_character.insert(query_word.substr(i, nbytes));
        
        i += nbytes;
    }

    vector<set<int> *> ch_set_vec;
    for(string ch : unrepeated_character)
    {
        auto idx_map_it = _word_idx_map.find(ch);
        if(idx_map_it != _word_idx_map.end())
        {
            ch_set_vec.push_back(&idx_map_it->second);
        }
    }

    for(size_t i = 0; i < ch_set_vec.size(); i++)
    {
        for(auto x : *ch_set_vec[i])
        {
            ret.insert(x);
        }
    }

    return ret;
}

priority_queue<candidateWord> KeyRecommend::genereateResult(string query_word, set<int> & candidate_set)
{
    priority_queue<candidateWord> pq;
    auto word_freq_dict = Dictionary::getInstance()->getWordFreqDict();
    
    for(auto word_idx : candidate_set)
    {
        auto cur_word = word_freq_dict[word_idx].first;
        auto cur_word_freq = word_freq_dict[word_idx].second;

        if(cur_word.size() <= query_word.size() * 2)
        {
            candidateWord w;
            w.word = cur_word;
            w.distance = minEditDistance(query_word, cur_word);
            w.hot = cur_word_freq;

            if(pq.size() < 10)
            {
                pq.push(w);
            }
            else
            {
                if(w.distance < pq.top().distance)
                {
                    pq.pop();
                    pq.push(w);
                }
                else if(w.distance == pq.top().distance && w.hot > pq.top().hot)
                {
                    pq.pop();
                    pq.push(w);
                }
            }
        }
    }

    return pq;
}

size_t KeyRecommend::nByteCode(const char ch)
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

size_t KeyRecommend::length(const string & str)
{
    size_t ilen = 0;

    for(size_t idx = 0; idx != str.size(); ++idx)
    {
        int nBytes = nByteCode(str[idx]);
        idx += (nBytes - 1);
        ++ilen;
    }

    return ilen;
}

int KeyRecommend::triple_min(const int & lhs, const int &mhs, const int & rhs)
{
    return lhs < mhs ? (lhs < rhs ? lhs : rhs) : (mhs < rhs ? mhs : rhs);
}

int KeyRecommend::minEditDistance(string first_word, string second_word)
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
        size_t bytes_num = nByteCode(first_word[first_idx]);
        first_sub_str = first_word.substr(first_idx, bytes_num);
        first_idx += bytes_num;

        for(size_t j = 1, second_idx = 0; j <= second_word_len; j++)
        {
            // 3.2 得到second_word的第一个人类视角字符及其所占字节数
            bytes_num = nByteCode(second_word[second_idx]);
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

    return dp[first_word_len][second_word_len];
}