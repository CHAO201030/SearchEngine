#ifndef __DOUBLE_CACHE_H__
#define __DOUBLE_CACHE_H__

#include "Cache.h"
#include "Configuration.h"
#include <mutex>

using std::mutex;
using std::lock_guard;

class DoubleCache;

class SubCache
{
friend class DoubleCache;

public:
    SubCache();

    pair<string, string> getKV(string key);

    bool setKV(string key, string value);

    unordered_map<string, list<pair<string, string>>::iterator> & getSearchMap();

    list<pair<string, string>> & getCacheContent();

private:
    bool noSwapOut(pair<string, string> key_value);

    void updataCacheOrder(pair<string, string> key_value);

private:
    int _cache_size;

    bool _is_lock;

    mutex _cache_lock;

    unordered_map<string, list<pair<string, string>>::iterator> _search_map;

    list<pair<string, string>> _cache_content;
};

class DoubleCache : public Cache
{
public:
    DoubleCache();

    virtual pair<string, string> getKV(string key) override;

    virtual bool setKV(string key, string value) override;

    virtual bool updataCache() override;

    virtual list<pair<string, string>> & getPendingList();

    virtual mutex & getCacheLock();

public:
    void swapCache();

private:
    SubCache  _read_cache;
    
    SubCache  _syn_cache;
};

#endif
