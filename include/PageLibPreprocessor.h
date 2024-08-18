#ifndef __PAGE_LIB_PREPROCESSOR_H__
#define __PAGE_LIB_PREPROCESSOR_H__

#include "Configuration.h"
#include "SplitTool.h"
#include "SplitToolCppJieba.h"
#include "WebPage.h"
#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <string>
#include <unordered_map>
#include <utility>
#include <set>

using std::set;
using std::vector;
using std::string;
using std::unordered_map;
using std::pair;
using std::ifstream;
using std::ofstream;
using std::istringstream;
using std::ostringstream;
using std::cin;
using std::cout;
using simhash::Simhasher;

class PageLibPreprocessor
{
using TF_DF_MAP = unordered_map<string, vector<pair<int, int>>>;
using WEIGHT_VES = vector<vector<pair<string, double>>>;
public:
    PageLibPreprocessor();

    void generateUnRepeatedWebPageLib();

    void generateInvertIndexLib();

private:
    void loadOriginalWebPageOffsetLib();

    void unrepeatedHelper();

    bool isDuplicateWebPage(uint64_t cur_page_figure_print);

    void storeIntoWebPageLib(ofstream & ofs, WebPage & cur_page);

    void storeIntoOffsetLib(ofstream & ofs, int doc_id, int offset, int page_len);

private:
    void loadUnrepeatedWebPageOffsetLib();

    void invertIndexHelper();

    void saveWordTfDf(TF_DF_MAP & tf_df_umap, map<string, int> & word_frequence_map, int doc_id);

    void calculateTFIDF(TF_DF_MAP & tf_df_umap, WEIGHT_VES & doc_weight_ves);

    void minMaxScaler(WEIGHT_VES & doc_weight_ves);

    void storeIntoInvertIndexLib(ofstream & ofs);

private:
    SplitTool * _p_split_tool;  // 分词工具
    
    Simhasher *_p_hasher;   // 去重工具

    unordered_map<int, pair<int, int>> _offset_lib; // 网页偏移库

    unordered_map<string, set<pair<int, double>>> _invert_index_lib;  // 去重网页倒排索引库

    vector<uint64_t> _page_figure_print_ves;
};

#endif
