#ifndef __DIR_SCANNER_H__
#define __DIR_SCANNER_H__

#include "Configuration.h"
#include <vector>
#include <string>
#include <sys/types.h>
#include <dirent.h>
#include <string.h>
#include <iostream>

using std::vector;
using std::string;

class DirScanner
{
public:
    vector<string> & getFiles();

    void traverse(string dir);

private:
    vector<string> _files;
};

#endif
