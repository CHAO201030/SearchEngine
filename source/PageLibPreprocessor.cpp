#include "../include/PageLibPreprocessor.h"
#include <math.h>

using TF_DF_MAP = unordered_map<string, vector<pair<int, int>>>;
using WEIGHT_VES = vector<vector<pair<string, double>>>;

PageLibPreprocessor::PageLibPreprocessor()
{
    string dictPath  = Configuration::getInstance()->getConfig("DICT_PATH");
    string modelPath = Configuration::getInstance()->getConfig("HMM_PATH");
    string idfPath   = Configuration::getInstance()->getConfig("IDF_PATH");
    string stopWords = Configuration::getInstance()->getConfig("STOP_WORD_PATH");

    _p_split_tool = new SplitToolCppJieba();
    _p_hasher     = new Simhasher(dictPath, modelPath, idfPath, stopWords);
}

void PageLibPreprocessor::generateUnRepeatedWebPageLib()
{
    loadOriginalWebPageOffsetLib();

    unrepeatedHelper(); 
}

void PageLibPreprocessor::loadOriginalWebPageOffsetLib()
{
    cout << "[INFO] : Load Original Web Page Offset Lib...\n";

    // ifstream offset_lib_ifs(Configuration::getInstance()->getConfig("test_offsetlib.dat"));

    ifstream offset_lib_ifs(Configuration::getInstance()->getConfig("offsetlib.dat"));

    string line;

    while(getline(offset_lib_ifs, line))
    {
        istringstream iss(line);

        int doc_id = 0, offset = 0, page_len = 0;

        iss >> doc_id >> offset >> page_len;

        _offset_lib.insert({doc_id, {offset, page_len}});    
    }

    cout << "[INFO] : Load Original Web Page Offset Lib Success...\n";
}

void PageLibPreprocessor::unrepeatedHelper()
{
    cout << "[INFO] : Generate Unrepeated Web Page Lib And Offset Lib...\n";

    cout << "[INFO] : Remove Duplicate Web Page...\n";

    // ifstream web_page_lib_ifs(Configuration::getInstance()->getConfig("test_webpagelib.dat"));
    // ofstream unrepeated_web_page_lib_ofs(Configuration::getInstance()->getConfig("test_unrepeated_webpage.dat"));
    // ofstream unrepeated_offset_lib_ofs(Configuration::getInstance()->getConfig("test_unrepeated_offsetlib.dat"));
    
    ifstream web_page_lib_ifs(Configuration::getInstance()->getConfig("webpage.dat"));
    ofstream unrepeated_web_page_lib_ofs(Configuration::getInstance()->getConfig("unrepeated_webpage.dat"));
    ofstream unrepeated_offset_lib_ofs(Configuration::getInstance()->getConfig("unrepeated_offsetlib.dat"));

    for(size_t i = 0; i < _offset_lib.size(); i++)
    {
        double percentage = double(100) * i / _offset_lib.size();        
        printf("\t-> %5.2f%% : %ld\r", percentage, i);
        fflush(stdout);

        int doc_id   = i + 1;
        int offset   = _offset_lib[doc_id].first;
        int page_len = _offset_lib[doc_id].second;

        char * p_buf = new char [page_len + 1] ();
    
        web_page_lib_ifs.seekg(offset);

        web_page_lib_ifs.read(p_buf, page_len);

        string cur_page_content(p_buf);

        WebPage cur_page(cur_page_content);

        cur_page.unrepeatedWebPageLibHelper(_p_hasher);

        uint64_t cur_page_figure_print = cur_page.getFigurePrint();

        if(isDuplicateWebPage(cur_page_figure_print))
        {
            continue;
        }
        else
        {
            _page_figure_print_ves.emplace_back(cur_page_figure_print);
            storeIntoWebPageLib(unrepeated_web_page_lib_ofs, cur_page);
            storeIntoOffsetLib(unrepeated_offset_lib_ofs, _page_figure_print_ves.size(), offset, page_len);
        }

        delete [] p_buf;
    }

    cout << "\t-> 100.00% : " << _offset_lib.size() << " --> " << _page_figure_print_ves.size() << "\n";
    cout << "[INFO] : Remove Duplicate Web Page Success...\n";
    cout << "[INFO] : Generate Unrepeated Web Page Lib And Offset Lib Success...\n";
}

bool PageLibPreprocessor::isDuplicateWebPage(uint64_t cur_page_figure_print)
{
    for(size_t i = 0; i < _page_figure_print_ves.size(); i++)
    {
        if(Simhasher::isEqual(cur_page_figure_print, _page_figure_print_ves[i]))
        {
            return true;
        }
    }

    return false;
}

void PageLibPreprocessor::storeIntoWebPageLib(ofstream & ofs, WebPage & cur_page)
{
    ofs << "<webpage>\n"
        << "\t<docid>"   << _page_figure_print_ves.size() << "</docid>\n"
        << "\t<title>"   << cur_page.getTitle() << "</title>\n"
        << "\t<link>"    << cur_page.getURL() << "</link>\n"
        << "\t<content>" << cur_page.getContent() << "</content>\n"
        << "</webpage>\n";

}

void PageLibPreprocessor::storeIntoOffsetLib(ofstream & ofs, int doc_id, int offset, int page_len)
{
    static int new_offset = 0;
    ofs << doc_id << " " << new_offset << " " << page_len << "\n";
    new_offset += page_len + 1;
}



void PageLibPreprocessor::generateInvertIndexLib()
{
    loadUnrepeatedWebPageOffsetLib();

    invertIndexHelper();
}

void PageLibPreprocessor::loadUnrepeatedWebPageOffsetLib()
{
    cout << "[INFO] : Load Unrepeated Web Page Offset Lib...\n";

    // ifstream offset_lib_ifs(Configuration::getInstance()->getConfig("test_unrepeated_offsetlib.dat"));

    ifstream offset_lib_ifs(Configuration::getInstance()->getConfig("unrepeated_offsetlib.dat"));

    string line;

    while(getline(offset_lib_ifs, line))
    {
        istringstream iss(line);

        int doc_id = 0, offset = 0, page_len = 0;

        iss >> doc_id >> offset >> page_len;

        _offset_lib.insert({doc_id, {offset, page_len}});    
    }

    cout << "[INFO] : Load Unrepeated Web Page Offset Lib Success...\n";
}

void PageLibPreprocessor::invertIndexHelper()
{
    cout << "[INFO] : Generate Invert Index Lib...\n";
    cout << "[INFO] : Load Unrepeated Web Page Lib...\n";

    // ifstream unrepeated_web_page_lib_ifs(Configuration::getInstance()->getConfig("test_unrepeated_webpage.dat"));
    // ofstream invert_index_lib_ofs(Configuration::getInstance()->getConfig("test_invert_index_lib.dat"));

    ifstream unrepeated_web_page_lib_ifs(Configuration::getInstance()->getConfig("unrepeated_webpage.dat"));
    ofstream invert_index_lib_ofs(Configuration::getInstance()->getConfig("invert_index_lib.dat"));

    TF_DF_MAP tf_df_umap;

    WEIGHT_VES doc_weight_ves(_offset_lib.size() + 1);

    for(size_t i =0; i < _offset_lib.size(); i++)
    {
        double percentage = double(100) * i / _offset_lib.size();        
        printf("\t-> %5.2f%% : %ld\r", percentage, i);
        fflush(stdout);

        int doc_id   = i + 1;
        int offset   = _offset_lib[doc_id].first;
        int page_len = _offset_lib[doc_id].second;

        char * p_buf = new char [page_len + 1] ();

        unrepeated_web_page_lib_ifs.seekg(offset);

        unrepeated_web_page_lib_ifs.read(p_buf, page_len);

        string cur_page_content(p_buf);

        WebPage cur_page(cur_page_content);

        cur_page.invertIndexLibHelper(_p_split_tool);

        saveWordTfDf(tf_df_umap, cur_page.getWordFrequenceMap(), doc_id);

        delete [] p_buf;
    }

    cout << "\t-> 100.00%\n";
    cout << "[INFO] : Load Unrepeated Web Page Lib Success...\n";

    cout << "[INFO] : Calculate Web Page Word's TF-IDF...\\n";

    calculateTFIDF(tf_df_umap, doc_weight_ves);

    cout << "\t-> Calculate Success...\n" << "\t-> Min-Max-Scaler Web Page Word's TF-IDF\n";
    
    minMaxScaler(doc_weight_ves);
    
    cout << "\t-> Min-Max-Scaler Web Page Word's TF-IDF Success\n";

    cout << "[INFO] : Calculate Web Page Word's TF-IDF Success...\\n";

    storeIntoInvertIndexLib(invert_index_lib_ofs);

    cout << "[INFO] : Generate Invert Index Lib Success...\n";
}

void PageLibPreprocessor::saveWordTfDf(TF_DF_MAP & tf_df_umap, map<string, int> & word_frequence_map, int doc_id)
{
    for(auto & it : word_frequence_map)
    {
        tf_df_umap[it.first].emplace_back(doc_id, it.second);
    }
}

void PageLibPreprocessor::calculateTFIDF(TF_DF_MAP & tf_df_umap, WEIGHT_VES & doc_weight_ves)
{

    size_t i = 0;

    for(auto & it : tf_df_umap)
    {
        double percentage = double(100) * i / tf_df_umap.size();        
        printf("\t  -> %5.2f%% : %ld\r", percentage, i);
        fflush(stdout);

        size_t N  = _offset_lib.size();
        size_t DF = it.second.size();

        for(size_t i = 0; i < DF; i++)
        {
            size_t TF = it.second[i].second;
            size_t doc_id = it.second[i].first;

            double temp = double(N)/(DF + 1) + double(1);   // 1 for IDF is positive number  eg, log2(1) = 0

            double IDF = log2(temp);

            double w = TF * IDF;

            doc_weight_ves[doc_id].emplace_back(it.first, w);
        }
    }

    printf("\t  -> 100.00%%\n");
}

void PageLibPreprocessor::minMaxScaler(WEIGHT_VES & doc_weight_ves)
{
    for(size_t doc_id = 1; doc_id < doc_weight_ves.size(); doc_id++)
    {   
        double sigma_weight_squre = 0;
        
        for(size_t word_count = 0; word_count < doc_weight_ves[doc_id].size(); word_count++)
        {
            double word_weight = doc_weight_ves[doc_id][word_count].second;
            sigma_weight_squre += (word_weight * word_weight);
        }

        for(size_t word_count = 0; word_count < doc_weight_ves[doc_id].size(); word_count++)
        {
            double & word_weight = doc_weight_ves[doc_id][word_count].second;
            word_weight = word_weight/sqrt(sigma_weight_squre);

            string  & cur_word = doc_weight_ves[doc_id][word_count].first;

            _invert_index_lib[cur_word].insert({doc_id, word_weight});
        }
    }
}

void PageLibPreprocessor::storeIntoInvertIndexLib(ofstream & ofs)
{
    for(auto & umit : _invert_index_lib)
    {
        ofs << umit.first;

        for(auto & sit : umit.second)
        {
            ofs << " " << sit.first << " " << sit.second;
        }

        ofs << "\n";
    }
}

// Test Unit    ----> PASS
// int main(void)
// {
//     PageLibPreprocessor plp;
//     // plp.generateUnRepeatedWebPageLib();
//     // plp.generateInvertIndexLib();
// }
