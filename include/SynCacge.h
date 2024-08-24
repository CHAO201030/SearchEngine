#ifndef __SYN_CACHE_H__
#define __SYN_CACHE_H__

#include "Cache.h"
#include "Configuration.h"
#include <mutex>

using std::mutex;
using std::lock_guard;

class SynCache : public Cache
{
public:
    SynCache():Cache()
    {
        _cache_size = std::stoi(Configuration::getInstance()->getConfig("CacheSize"));
    }

    virtual pair<string, string> getKV(string key) override;
    virtual bool setKV(string key, string value) override;
    virtual bool updataCache() override;
    virtual list<pair<string, string>> & getPendingList() override;
    virtual mutex & getCacheLock() override;
public:

private:
    bool noSwapOut(pair<string, string> key_value);
    void updataCacheOrder(pair<string, string> key_value);

private:
    unordered_map<string, list<pair<string, string>>::iterator> _search_map;

    list<pair<string, string>> _cache_content;

    list<pair<string, string>> _pending_list;

    mutex _lock;

    int _cache_size;

    bool _wait_update = false;  // 第一次没读到 需要在写的时候更新到pending_list中
};

#endif
