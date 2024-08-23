#ifndef __CACHE_H__
#define __CACHE_H__

#include <string>
#include <utility>
#include <unordered_map>
#include <list>
#include <mutex>

using std::string;
using std::pair;
using std::unordered_map;
using std::list;
using std::mutex;

class Cache
{
public:
    virtual pair<string, string> getKV(string key) = 0;
    virtual bool setKV(string key, string value) = 0;
    virtual bool updataCache() = 0;
    virtual list<pair<string, string>> & getPendingList() = 0;
    virtual mutex & getCacheLock() = 0;
};

#endif
