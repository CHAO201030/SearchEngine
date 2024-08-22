#ifndef __BIG_CACHE_H__
#define __BIG_CACHE_H__

#include "Cache.h"
#include "Configuration.h"
#include <mutex>

using std::mutex;
using std::lock_guard;

class BigCache : public Cache
{
public:
    static Cache * getInstance();

public:
    virtual pair<string, string> getKV(string key) override;
    virtual bool setKV(string key, string value) override;
    virtual bool updataCache() override;

private:
    BigCache();

    BigCache(const BigCache & rhs) = delete;
    BigCache(BigCache && rhs) = delete;

    BigCache & operator=(const BigCache & rhs) = delete;
    BigCache & operator=(BigCache && rhs) = delete;

private:
    bool noSwapOut(pair<string, string> key_value);
    void updataCacheOrder(pair<string, string> key_value);
private:
    static Cache * _p_cache;

    unordered_map<string, list<pair<string, string>>::iterator> _search_map;

    list<pair<string, string>> _cache_content;

    mutex _lock;

    int _cache_size;
};

#endif
