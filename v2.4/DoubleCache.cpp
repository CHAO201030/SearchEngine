#include "../include/DoubleCache.h"

DoubleCache::DoubleCache():Cache()
{

}

pair<string, string> DoubleCache::getKV(string key)
{
    return _read_cache.getKV(key);
}

bool DoubleCache::setKV(string key, string value)
{
    return _read_cache.setKV(key, value);
}

bool DoubleCache::updataCache()
{
    swapCache();

    return true;
}

void DoubleCache::swapCache()
{
    // 交换 syn_cache 和 read_cache 的内容
    
    lock_guard<mutex> read_lg(_read_cache._cache_lock);

    std::cout << "\t\tBefore Swap read : ";
    
    for(auto lit = _read_cache._cache_content.begin(); lit != _read_cache._cache_content.end(); ++lit)
    {
        std::cout << lit->first << " ";
    }

    std::cout << "\n\t\tBefore Swap syn  : ";

    for(auto lit = _syn_cache._cache_content.begin(); lit != _syn_cache._cache_content.end(); ++lit)
    {
        std::cout << lit->first << " ";
    }
    std::cout << "\n";


    std::swap(_read_cache._cache_content, _syn_cache._cache_content);
    std::swap(_read_cache._search_map, _syn_cache._search_map);
    

    std::cout << "\t\tAfter  Swap read : ";
    for(auto lit = _read_cache._cache_content.begin(); lit != _read_cache._cache_content.end(); ++lit)
    {
        std::cout << lit->first << " ";
    }
    std::cout << "\n\t\tAfter  Swap syn  : ";

    for(auto lit = _syn_cache._cache_content.begin(); lit != _syn_cache._cache_content.end(); ++lit)
    {
        std::cout << lit->first << " ";
    }
    std::cout << "\n";
}

list<pair<string, string>> & DoubleCache::getPendingList()
{
    return _syn_cache._cache_content;
}

mutex & DoubleCache::getCacheLock()
{
    return _syn_cache._cache_lock;
}


/***********************************************************************************************************************************************/


SubCache::SubCache()
{
    _cache_size = std::stoi(Configuration::getInstance()->getConfig("CacheSize"));
}

pair<string, string> SubCache::getKV(string key)
{
    lock_guard<mutex> lg(_cache_lock);

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

bool SubCache::setKV(string key, string value)
{
    lock_guard<mutex> lg(_cache_lock);

    updataCacheOrder({key, value});

    return true;
}

bool SubCache::noSwapOut(pair<string, string> key_value)
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

void SubCache::updataCacheOrder(pair<string, string> key_value)
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

unordered_map<string, list<pair<string, string>>::iterator> & SubCache::getSearchMap()
{
    return _search_map;
}

list<pair<string, string>> & SubCache::getCacheContent()
{
    return _cache_content;
}
