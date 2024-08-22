#include "../include/BigCache.h"

Cache * BigCache::_p_cache = nullptr;

BigCache::BigCache()
{
    _cache_size = std::stoi(Configuration::getInstance()->getConfig("CacheSize"));
    std::cout << _cache_size << "\n";
}

Cache * BigCache::getInstance()
{
    if(_p_cache == nullptr)
    {
        _p_cache = new BigCache();
    }

    return _p_cache;
}

bool BigCache::updataCache()
{
    return true;
}

pair<string, string> BigCache::getKV(string key)
{
    lock_guard<mutex> cache_lock(_lock);

    auto umit = _search_map.find(key);

    if(umit == _search_map.end())
    {
        return {"", ""};
    }
    else
    {
        auto lit = umit->second;

        noSwapOut(*lit);

        return *umit->second;
    }
}

bool BigCache::setKV(string key, string value)
{
    lock_guard<mutex> cache_lock(_lock);

    updataCacheOrder({key, value});

    return true;
}

bool BigCache::noSwapOut(pair<string, string> key_value)
{
    auto umit = _search_map.find(key_value.first);

    if(umit != _search_map.end())
    {
        auto lit = umit->second;

        if(lit->first == key_value.first)
        {
            _cache_content.splice(_cache_content.begin(), _cache_content, lit);

            umit->second = _cache_content.begin();

            return true;
        }
    }

    return false;
}

void BigCache::updataCacheOrder(pair<string, string> key_value)
{    
    if(_cache_content.size() < _cache_size)
    {
        if(!noSwapOut(key_value))
        {
            _cache_content.push_front(key_value);

            auto lit = _cache_content.begin();
            _search_map[key_value.first] = lit;
        }
    }
    else
    {
        if(!noSwapOut(key_value))
        {
            auto erase_it = _cache_content.rbegin();
            _search_map.erase(erase_it->first);

            _cache_content.pop_back();
            _cache_content.push_front(key_value);

            auto lit = _cache_content.begin();
            _search_map[key_value.first] = lit;
        }
    }
}
