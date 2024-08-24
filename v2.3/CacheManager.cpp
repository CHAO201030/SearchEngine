#include "../include/CacheManager.h"

Cache * CacheManager::getThreadCache(pid_t thread_id)
{
    auto mit = _tid_cache_map.find(thread_id);

    if(mit != _tid_cache_map.end())
    {
        return mit->second;
    }
    else
    {
        _tid_cache_map[thread_id] = new SynCache();

        return _tid_cache_map.find(thread_id)->second;
    }
}

void CacheManager::updateAllThreadCache()
{
    std::cout << "[INFO] : Unpdate All Cache...\n";

    // 1. 找到主节点
    auto umit = _tid_cache_map.begin();
    if(umit == _tid_cache_map.end())
    {
        std::cout << "\tNo Cache Info...\n";
        return ;
    }

    // 2. 获取主节点的 pendinglist
    list<pair<string, string>> main_pending_list = umit->second->getPendingList();

    mutex & main_cache_mutex =  umit->second->getCacheLock();

    auto umit_4_main_cache = umit;

    list<pair<string, string>> temp_list;

    // 3. 合并
    while(umit != _tid_cache_map.end())
    {
        mutex & thread_cache_mutex = umit->second->getCacheLock();

        lock_guard<mutex> lg(thread_cache_mutex);

        list<pair<string, string>> & thread_pending_list = umit->second->getPendingList();

        temp_list.insert(temp_list.end(), thread_pending_list.begin(), thread_pending_list.end());

        generateOperateSequence(temp_list);

        std::cout << "\t\tthread[ " << umit->first << " ] -> Add To Global Pending List Success...\n";

        ++umit;
    }

    // 4. 广播
    umit = _tid_cache_map.begin();

    while(umit != _tid_cache_map.end())
    {
        mutex & thread_cache_mutex = umit->second->getCacheLock();

        lock_guard<mutex> lg(thread_cache_mutex);

        auto t_lit = temp_list.begin();

        while(t_lit != temp_list.end())
        {
            umit->second->setKV(t_lit->first, t_lit->second);
            ++t_lit;
        }

        std::cout << "\t\tthread[ " << umit->first << " ] -> Update Cache Success...\n";

        ++umit;
    }
}

void CacheManager::generateOperateSequence(list<pair<string, string>> & operator_sequence_list)
{
    int cache_size = std::stoi(Configuration::getInstance()->getConfig("CacheSize"));
    
    unordered_map<string, list<pair<string, string>>::iterator> search_map;
    
    list<pair<string, string>> res_list; 
    
    auto lit = operator_sequence_list.begin();

    while(lit != operator_sequence_list.end())
    {
        // LRU
        auto mit = search_map.find(lit->first);

        if(mit == search_map.end())
        {
            // 如果cache中没有这个数据
            if(res_list.size() < cache_size)
            {
                // cache没满
                search_map[lit->first] = lit;
                res_list.push_front({lit->first, lit->second});
            }
            else
            {
                // cache满了
                auto res_back_it = res_list.rbegin();
                search_map.erase(res_back_it->first);
                res_list.pop_back();
                res_list.push_front({lit->first, lit->second});
            }
        }
        else
        {
            // 如果cache中有这个数据
            auto cur_it = mit->second;

            res_list.splice(res_list.begin(), res_list, cur_it);

            search_map[lit->first] = res_list.begin();
        }

        ++lit;
    }
}
