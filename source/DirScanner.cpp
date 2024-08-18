#include "../include/DirScanner.h"

vector<string> & DirScanner::getFiles()
{
    return _files;
}

void DirScanner::traverse(string dir)
{
    DIR * p_dir = opendir(dir.c_str());

    if(p_dir == nullptr)
    {
        perror("DirScanner::traverse -> opendir");
        exit(-1);
    }

    struct dirent * pdirent = nullptr;

    while((pdirent = readdir(p_dir)) != nullptr)
    {
        if(strcmp(pdirent->d_name, ".") == 0 || strcmp(pdirent->d_name, "..") == 0)
        {
            continue;
        }

        if(pdirent->d_type == DT_DIR)
        {
            traverse(dir + "/" + pdirent->d_name);
        }else
        {
            _files.emplace_back(dir + "/" + pdirent->d_name);
        }
    }
}

// Test Unit    ----> PASS
// int main(void)
// {
//     DirScanner scanner;
//     string zh_dir = Configuration::getInstance()->getConfig("chinese_corpus_dir");
//     scanner.traverse(zh_dir);
//     auto ves = scanner.getFiles();
//     for(auto it:ves)
//     {
//         std::cout << it << "\n";
//     }
//     return 0;
// }
