#include <cpp58.hpp>
#include "../include/simhash/cppjieba/Jieba.hpp"
// bool isChinese(char ch)
// {
//     return 0x80 & ch;
// }
// size_t nBytesCode(char ch)
// {
//     if(ch & 0x80)
//     {
//         int bytes = 1;
        
//         for(int i = 0; i != 6; i++)
//         {
//             if(ch & (1 << (6 - i)))
//             {
//                 bytes++;
//             }
//             else
//             {
//                 break;
//             }
//         }

//         return bytes;
//     }

//     return 1;
// }
// void extractChineseWord(string & word)
// {
//     for(size_t i = 0; i < word.size();)
//     {
//         size_t bytes = nBytesCode(word[i]);

//         if(!isChinese(word[i]))
//         {
//             word.erase(i, bytes);
//             continue;
//         }

//         i += bytes;
//     }
// }
int main()
{
    // 示例HTML字符串，包含两个相邻的<p>标签和它们之间的空格
    // std::string html = "<p style=\"text-align: center;\">    1 1   4<img alt=\"\" src=\"/NMediaFile/2020/1016/MAIN202010161709000287816714389.jpg\" style=\"width: 600px; height: 5473px;\" /></p>";
    // std::regex pattern1("<[^>]*>");
    // std::regex pattern2(R"(^\s*)");
    // // 替换空格为空字符串，即去除空格
    // std::string replacement = "";
    // html = std::regex_replace(html, pattern1, replacement);
    // std::cout << html << std::endl;
    // html = std::regex_replace(html, pattern2, replacement);
    // // 输出处理后的HTML字符串
    // std::cout << html << std::endl;

    // string content("啊iOS单打拿电脑，艾萨克牛顿卡萨丁123！啊实打实的3开45口了");

    // istringstream iss(content);

    // string temp;

    // extractChineseWord(content);



    // cout << content << "\n";
    /*
    string q = "adasad\n\t123123213<title>司法部、国家统计局负责人就《国务院关于修改<全国经济普查条例>的决定》有关问题答记者问</title>1231231231gfdsfsdf";

    auto tp = q.find("<title>");
    auto te = q.find("</title>");
    auto ignosize = q.substr(te, q.size()).size();

    string s = q.substr(tp, q.substr(tp).size() - ignosize + 8);
    cout << q << " " << q.size() << "\n";
    cout << s << " " << s.size() << "\n";
    
    auto beginpos = s.find("<title>");
    auto endpos   = s.find("</title>");

    auto t = s.substr(beginpos + 7, s.size() - 15);
    auto leftbeginpos = t.find("<");
    auto rightendpos = t.find(">");

    cout << s[leftbeginpos + 7] << " " << s[rightendpos + 7] << "\n";
    cout << t << "\n" << t[leftbeginpos] << " " << t[rightendpos] << "\n";
    cout << leftbeginpos << " " << rightendpos << "\n";

    q.erase(tp + leftbeginpos + 7, 1);
    q.erase(tp + rightendpos + 6, 1);

    cout << q << "\n";


    string _format_xml = "adasad\n\t123123213<title>司法部、国家统计局负责人就《国务院关于修改<全国经济普查条例>的决定》有关问题答记者问</title>1231231231gfdsfsdf";

    size_t title_begin = _format_xml.find("<title>");
    size_t title_end   = _format_xml.find("</title>");
    size_t ignore_size = _format_xml.substr(title_end, _format_xml.size()).size();

    string title_tag = _format_xml.substr(title_begin, _format_xml.substr(title_begin).size() - ignore_size + 8);

    string process_title = title_tag.substr(title_tag.find("<title>") + 7, title_tag.size() - 15);

    size_t left_arrow = process_title.find("<");
    size_t right_arrow = process_title.find(">");

    _format_xml.erase(title_begin + left_arrow + 7, 1);
    _format_xml.erase(title_begin + right_arrow + 6, 1);

    boolalpha(cout);
    cout << _format_xml << (_format_xml == q) << "\n";
    */

    unordered_map<string, vector<pair<int, int>>> tf_df_map;
    map<string, int> wf_map1({{"hello", 1}, {"hi", 2}});
    map<string, int> wf_map2({{"hello", 99}, {"hi", 88}, {"tg", 520}});

    int i = 1;
    for(auto & it : wf_map1)
    {
        // vector<pair<int, int>> & doc_id_frequence_vec = tf_df_map[it.first];
        // doc_id_frequence_vec.emplace_back(i, it.second);

        tf_df_map[it.first].emplace_back(i, it.second);
    }

    i = 2;
    for(auto & it : wf_map2)    
    {
        // vector<pair<int, int>> & doc_id_frequence_vec = tf_df_map[it.first];
        // doc_id_frequence_vec.emplace_back(i, it.second);

        tf_df_map[it.first].emplace_back(i, it.second);
    }

    for(auto & it : tf_df_map)
    {
        cout << "word = " << it.first << "\n";
        
        size_t vec_size = it.second.size();
        for(size_t i = 0; i < vec_size; i++)
        {
            cout << "\tdoc_id = " << it.second[i].first 
                 << ", frequence = " << it.second[i].second << "\n";
        }
    }



    // std::unordered_map<std::string, std::unordered_map<std::string, int>> outerMap;
    // std::unordered_map<std::string, int> innerMap;

    // // 假设我们有一些数据要存储
    // innerMap["key1"] = 1;
    // innerMap["key2"] = 2;

    // // 将 innerMap 作为 outerMap 的值
    // outerMap["outerKey"] = innerMap;

    // // 访问 outerMap 的值
    // std::unordered_map<std::string, int>& innerMapRef = outerMap["outerKey"];
    // std::cout << innerMapRef["key1"] << std::endl; // 输出: 1
    // std::cout << innerMapRef["key2"] << std::endl; // 输出: 2
    return 0;
}