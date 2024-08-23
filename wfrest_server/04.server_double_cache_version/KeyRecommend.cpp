#include "../include/KeyRecommend.h"

KeyRecommend::KeyRecommend()
{

}

void KeyRecommend::doRecommend(string query_word)
{
    set<int> merge_set;

    getRecommendWordSet(query_word, merge_set);

    generateResult(query_word, merge_set);
}

string KeyRecommend::getResult()
{
    vector<string> recommend_words_ves(_pri_que.size());

    for(int i = _pri_que.size() - 1; i >= 0; i--)
    {
        recommend_words_ves[i] = _pri_que.top().first;
        _pri_que.pop();
    }

    nlohmann::json res_json;

    for(size_t i = 0; i < recommend_words_ves.size(); i++)
    {
        res_json.push_back(recommend_words_ves[i]);
    }

    return res_json.dump();
}

void KeyRecommend::getRecommendWordSet(string query_word, set<int> & merge_set)
{
    set<string> unrepeated_character;

    auto _word_index_map = Dictionary::getInstance()->getWordFequenceIndexMap();

    // 1. 分离单词
    for(size_t i = 0; i < query_word.size();)
    {
        size_t nbytes = nBytesCode(query_word[i]);

        unrepeated_character.insert(query_word.substr(i, nbytes));

        i += nbytes;
    }

    // 2. 找集合
    vector<set<int> * > ch_set_vec;

    for(string ch : unrepeated_character)
    {
        auto index_map_it = _word_index_map.find(ch);
        if(index_map_it != _word_index_map.end())
        {
            ch_set_vec.push_back(&index_map_it->second);
        }
    }

    // 3. 取并集
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
}

void KeyRecommend::generateResult(string query_word, set<int> & merge_set)
{
    auto _dict_freq_vec = Dictionary::getInstance()->getWordFrequenceDict();

    auto sit = merge_set.begin();
    while(sit != merge_set.end())
    {
        STR_DISTANCE_FREQUENCE word_d_f;
    
        if(query_word.size() * 2 >= _dict_freq_vec[*sit].first.size())
        {
            word_d_f.first = _dict_freq_vec[*sit].first;

            word_d_f.second.first = minEditDistance(query_word, _dict_freq_vec[*sit].first);

            word_d_f.second.second = _dict_freq_vec[*sit].second;

            if(_pri_que.size() >= 10)
            {
                if(word_d_f.second.first < _pri_que.top().second.first)
                {
                    _pri_que.pop();
                    _pri_que.push(word_d_f);
                }
            }
            else
            {
                _pri_que.push(word_d_f);
            }
        }
        ++sit;
    }
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

    return dp[first_word_len][second_word_len];
}

size_t KeyRecommend::length(const string & str)
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

int KeyRecommend::triple_min(const int & lhs, const int & mhs, const int & rhs)
{
    return lhs < mhs ? (lhs < rhs ? lhs : rhs) : (mhs < rhs ? mhs : rhs);
}

size_t KeyRecommend::nBytesCode(const char ch)
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
