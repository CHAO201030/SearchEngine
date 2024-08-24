#include "../include/SynCacge.h"

list<pair<string, string>> & SynCache::getPendingList()
{
    return _pending_list;
}

mutex & SynCache::getCacheLock()
{
    return _lock;
}

pair<string, string> SynCache::getKV(string key)
{
    auto umit = _search_map.find(key);

    if(umit == _search_map.end())
    {
        _wait_update = true;
        return {"", ""};
    }
    else
    {
        auto lit = umit->second;

        noSwapOut(*lit);

        _pending_list.push_back(*umit->second);

        return *umit->second;
    }
}

bool SynCache::setKV(string key, string value)
{
    updataCacheOrder({key, value});

    if(_wait_update)
    {
        _pending_list.push_back({key, value});
        _wait_update = false;
    }

    return true;
}

bool SynCache::updataCache()
{
    return true;
}

bool SynCache::noSwapOut(pair<string, string> key_value)
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

void SynCache::updataCacheOrder(pair<string, string> key_value)
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
