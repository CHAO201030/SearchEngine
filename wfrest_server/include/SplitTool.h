#ifndef __SPLIT_TOOL_H__
#define __SPLIT_TOOL_H__

#include <vector>
#include <string>

class SplitTool
{
public:
    virtual std::vector<std::string> cut(const std::string & file_content) = 0;
};

#endif
