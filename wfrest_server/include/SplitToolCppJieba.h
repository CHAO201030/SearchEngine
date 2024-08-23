#ifndef __SPLIT_TOOL_CPP_JIEBA_H__
#define __SPLIT_TOOL_CPP_JIEBA_H__

#include "Configuration.h"
#include "SplitTool.h"
#include "simhash/cppjieba/Jieba.hpp"

#include <vector>
#include <string>

using std::vector;
using std::string;

class SplitToolCppJieba : public SplitTool
{
public:
    SplitToolCppJieba();

    ~SplitToolCppJieba();

    virtual vector<string> cut(const string & file_content) override;

private:
    cppjieba::Jieba * _jieba;
};

#endif
