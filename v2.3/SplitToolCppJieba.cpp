#include "../include/SplitToolCppJieba.h"

SplitToolCppJieba::SplitToolCppJieba()
:_jieba(nullptr)
{
    if(_jieba == nullptr)
    {
        string DICT_PATH = Configuration::getInstance()->getConfig("DICT_PATH");
        string HMM_PATH = Configuration::getInstance()->getConfig("HMM_PATH");
        string USER_DICT_PATH = Configuration::getInstance()->getConfig("USER_DICT_PATH");
        string IDF_PATH = Configuration::getInstance()->getConfig("IDF_PATH");
        string STOP_WORD_PATH = Configuration::getInstance()->getConfig("STOP_WORD_PATH");

        _jieba = new cppjieba::Jieba(DICT_PATH, HMM_PATH, USER_DICT_PATH, IDF_PATH, STOP_WORD_PATH);
    }
}

SplitToolCppJieba::~SplitToolCppJieba()
{
    if(_jieba)
    {
        delete _jieba;
        _jieba = nullptr;
    }
}

vector<string> SplitToolCppJieba::cut(const string & file_content)
{
    vector<string> ret;

    _jieba->Cut(file_content, ret);

    return ret;
}

// Test Unit    ----> PASS
// int main(void)
// {
//     SplitToolCppJieba jb;
//     string s = "爱神的箭阿三吉拉是大家来看看吗";
//     vector<string> res = jb.cut(s);
//     for(auto str:res)
//     {
//         std::cout << str << "\n";
//     }
//     return 0;
// }
