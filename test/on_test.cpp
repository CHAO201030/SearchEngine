#include <cpp58.hpp>
#include "../include/nlohmann/json.hpp"
#include <workflow/WFTaskFactory.h>
#include <workflow/WFFacilities.h>

using namespace nlohmann;
#define TEST_COUNT 20

using STR_DISTANCE_FREQUENCE = pair<string, pair<int, int>>;

void loadDict(vector<pair<string, int>> & dict_vec);
void loadIndex(map<string, set<int>> & index_map);
void generateCandidateWordsSet(string word);
int minEditDistance(string first_word, string second_word);
size_t nBytesCode(const char ch);
std::size_t length(const std::string &str);
int triple_min(const int &a, const int &b, const int &c);

set<int> queryWordSetIntersection(vector<set<int> *> & set_ves);


template<class T>
ostream & operator<<(ostream & os, set<T> & rhs);

bool operator<(const STR_DISTANCE_FREQUENCE & lhs, const STR_DISTANCE_FREQUENCE & rhs)
{
    if(lhs.second.first < rhs.second.first)
    {
        cout << "1" <<"\n";
        return true;
    }
    else if(lhs.second.first == rhs.second.first)
    {
        if(lhs.second.second < rhs.second.second)
        {
            cout << "2\n";
            return true;
        }
    }

    cout << "3\n";
    return false;
}

struct SDF_Compare
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
            return true;
        }

        return false;
    }
};

vector<pair<string, int>> dict_vec;
map<string, set<int>> index_map;

WFFacilities::WaitGroup wg(1);

void timerFunc(WFTimerTask * timer_task)
{
    cout << "Time out\n";

    WFTimerTask * task = WFTaskFactory::create_timer_task(1,0, timerFunc);

    series_of(timer_task)->push_back(task);
}

ostream & operator<<(ostream & os, const list<int> & rhs)
{
    for(auto lit = rhs.begin(); lit != rhs.end(); ++lit)
    {
        os << *lit << " ";
    }

    return os;
}

int main(int argc, char **argv)
{
    // loadDict(dict_vec);
    //
    // loadIndex(index_map);
    //
    // string query_word;
    // cout << " >> ";
    // while(cin >> query_word)
    // {
    //     generateCandidateWordsSet(query_word);
    //     cout << " >> ";
    // }

    // cout << minEditDistance("hello", "hello") << "\n";

    // priority_queue<STR_DISTANCE_FREQUENCE> recommend_word_queue;
    // recommend_word_queue.push({"apple", {10, 5}});
    // recommend_word_queue.push({"banana", {5, 20}});
    // recommend_word_queue.push({"cherry", {10, 15}});
    //   while (!recommend_word_queue.empty()) {
    //     STR_DISTANCE_FREQUENCE top = recommend_word_queue.top();
    //     std::cout << top.first << " - (" << top.second.first << ", " << top.second.second << ")" << std::endl;
    //     recommend_word_queue.pop();
    // }
    //
    // STR_DISTANCE_FREQUENCE p1({"apple", {10, 5}});
    // STR_DISTANCE_FREQUENCE p2({"banana", {5, 20}});
    //
    // cout << (p2 < p1) << "\n";

    // vector<string> split_res = {"我", "北京大学", "哈哈", "它", "上海大学", "你", "的", "他们"};
    //
    // unordered_set<string> stop_words_set = {"哈哈", "它", "的", "他们"};
    //
    // auto it = split_res.begin();
    // while(it != split_res.end())
    // {
    //     if(stop_words_set.find(*it) != stop_words_set.end())
    //     {
    //         it = split_res.erase(it);
    //         continue;
    //     }
    //     ++it;
    // }

    // set<int> s1 = {1, 2, 3, 4, 5, 6, 7, 8, 9};
    // set<int> s2 = {11, 2, 31, 4, 51, 6, 7, 8, 9};
    // set<int> s3 = {1, 2, 13, 4, 5, 6, 17, 8, 19};
    //   
    // vector<set<int> *> set_ves = {&s1, &s2, &s3};
    //
    // set<int> query_word_set = queryWordSetIntersection(set_ves);
    //
    // cout << query_word_set << "\n";
    //
    // priority_queue<pair<int, double>> q;
    //
    // q.push(make_pair(10001, 1));
    // q.push(make_pair(10422, 0.984));
    // q.push(make_pair(10003, 0.999));
    //
    // for(size_t i = 0; i < 3; i++)
    // {
    //     cout << q.top().first << " " << q.top().second << "\n";
    //     q.pop();
    // }

    // vector<vector<string>> ves = {{"WD", "58", "SDU"}, {"WD", "http://gongyi.people.com.cn/n1/2021/0401/c151132-32067582.html", "MSU"}};
    //
    // json child_json;
    //
    // for(size_t i = 0; i < ves.size(); i++)
    // {
    //    
    //     string title = "<p>" + ves[i][0] + "</p>";
    //     string link = "<a href=\"" + ves[i][1] +"\">";
    //     string content = "<p>" + ves[i][2] + "</p>";
    //
    //     string res = "<div>" + link + title + "</a>" + content + "</div>";
    //     cout << res << "\n";
    //
    //     child_json.push_back(res);
    // }
    //
    // cout << "\n" << child_json.dump() << "\n";
    
    // WFTimerTask * timer_task = WFTaskFactory::create_timer_task(1,0, timerFunc);
    // timer_task->start();
    // wg.wait();


    list<int> l1 = {1, 2, 3, 4};
    list<int> l2 = {3, 4, 5};

    auto & a = l1;
    auto & b = l2;

    // swap(a, b);
    l1.clear();

    l1.insert(l1.end(), l2.begin(), l2.end());

    cout << l1 << "\n";
    cout << l2 << "\n";
    return 0;
}

int en_offset = 0;

set<int> queryWordSetIntersection(vector<set<int> *> & set_ves)
{
    set<int> res = *set_ves[0];

    for(size_t i = 1; i < set_ves.size(); i++)
    {
        vector<int> temp;
        set_intersection(res.begin(), res.end(), (*set_ves[i]).begin(), (*set_ves[i]).end(), back_inserter(temp));
        res = set<int>(temp.begin(), temp.end());
    }

    return res;
}

void loadDict(vector<pair<string, int>> & dict_vec)
{
    ifstream en_dict_ifs("../data/en_dict.dat");
    ifstream zh_dict_ifs("../data/zh_dict.dat");

    string line;

    int count = 0;

    while(getline(en_dict_ifs, line))
    {
        istringstream iss(line);
        
        string word;
        
        int frequence;

        iss >> word >> frequence;
    
        dict_vec.emplace_back(word, frequence);
    
        count++;
        // if(count == TEST_COUNT)break;
    }

    en_offset = dict_vec.size();
    cout << "en word num : " << en_offset << "\n";

    while(getline(zh_dict_ifs, line))
    {
        istringstream iss(line);
        
        string word;
        
        int frequence;

        iss >> word >> frequence;
    
        dict_vec.emplace_back(word, frequence);
    
        count++;
        // if(count == TEST_COUNT)break;
    }
}

void loadIndex(map<string, set<int>> & index_map)
{
    ifstream en_index_ifs("../data/en_dict_index.dat");
    ifstream zh_index_ifs("../data/zh_dict_index.dat");

    string line;

    while(getline(en_index_ifs, line))
    {
        istringstream iss(line);

        string word;
        
        int index;

        iss >> word;

        while(iss.good())
        {
            iss >> index;

            index_map[word].insert(index);
        }
    }

    while(getline(zh_index_ifs, line))
    {
        istringstream iss(line);

        string word;
        
        int index;

        iss >> word;

        while(iss.good())
        {
            iss >> index;

            index_map[word].insert(index + en_offset);
        }
    }
}

template<class T>
ostream & operator<<(ostream & os, set<T> & rhs)
{
    for(auto & str : rhs)
    {
        os << str << " ";
    }

    return os;
}

void generateCandidateWordsSet(string query_word)
{
    time_t begin = time(NULL);
    // 分词
    set<string> unrepeated_character;

    // TODO : 分离中英文混合输入
    //      1. 提取中英文 hello武汉hi -> hello, hi, 武汉 nByteCode和人类视角的中文长度
    //      2. 
    vector<string> en_query_word, zh_query_word;

    string en_word, zh_word;
/*
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
    for(size_t i = 0; i < query_word.size();)
    {
        size_t nbytes = nBytesCode(query_word[i]);

        unrepeated_character.insert(query_word.substr(i, nbytes));

        i += nbytes;
    }
    
    // 找集合
    vector<set<int> * > ch_set_vec;

    for(string ch : unrepeated_character)
    {
        auto index_map_it = index_map.find(ch);
        if(index_map_it != index_map.end())
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

    time_t end = time(NULL);

    cout << "Query character : " << unrepeated_character << "\n";
    cout << "Result set size = " << merge_set.size() << "\n";
    cout << "Use time = " << end - begin << "s\n";

    // 返回推荐词

    priority_queue<STR_DISTANCE_FREQUENCE, vector<STR_DISTANCE_FREQUENCE>, SDF_Compare> recommend_word_queue;

    auto sit = merge_set.begin();
    while(sit != merge_set.end())
    {
        STR_DISTANCE_FREQUENCE word_d_f;
    
        if(query_word.size() * 2 >= dict_vec[*sit].first.size())
        {
            word_d_f.first = dict_vec[*sit].first;

            word_d_f.second.first = minEditDistance(query_word, dict_vec[*sit].first);

            word_d_f.second.second = dict_vec[*sit].second;

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
    
    for(auto & str : recommend_words_ves)
    {
        cout << str << " ";
    }
    cout << "\n";
}


size_t nBytesCode(const char ch)
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

std::size_t length(const std::string &str)
{
    std::size_t ilen = 0;
    for(std::size_t idx = 0; idx != str.size(); ++idx)
    {
        int nBytes = nBytesCode(str[idx]);
        idx += (nBytes - 1);
        ++ilen;
    }
    return ilen;
}

int triple_min(const int &a, const int &b, const int &c)
{
    return a < b ? (a < c ? a : c) : (b < c ? b : c);
}

int minEditDistance(string first_word, string second_word)
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
