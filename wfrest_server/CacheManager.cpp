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
        // _tid_cache_map[thread_id] = new SynCache();

        _tid_cache_map[thread_id] = new DoubleCache();

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

        list<pair<string, string>> thread_pending_list = umit->second->getPendingList();

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

void CacheManager::updateAllThreadDoubleCache()
{
    std::cout << "[INFO] : Unpdate All Cache...\n";

    // 1. 找到主节点
    auto umit = _tid_cache_map.begin();
    if(umit == _tid_cache_map.end())
    {
        std::cout << "\tNo Cache Info...\n";
        return ;
    }

    auto main_umint = umit;

    Cache * main_cache = umit->second;

    // 2. 获得主节点的内容
    SubCache temp_cache;

    // auto main_cache_content = main_cache->getPendingList(); // 对于DoubleCache来说 PendingList 是 syn_cache 的内容

    // // 第一个节点是刚访问的 要保证 lru 顺序就要反向迭代
    // auto main_lit = main_cache_content.rbegin();
    // while(main_lit != main_cache_content.rend())
    // {
    //     temp_cache.setKV(main_lit->first, main_lit->second);
    //     ++main_lit;
    // }

    // 3. 合并从节点的内容
    // ++umit;
    while (umit != _tid_cache_map.end())
    {
        lock_guard<mutex> cur_cache_syn_lock(umit->second->getCacheLock());

        auto sub_lit = umit->second->getPendingList().rbegin();

        while(sub_lit != umit->second->getPendingList().rend())
        {
            temp_cache.setKV(sub_lit->first, sub_lit->second);
            ++sub_lit;
        }

        std::cout << "\t\tthread[ " << umit->first << " ] -> Merge To Main Cache Success...\n";

        ++umit;
    }

    std::cout << "\t\tGlobal Cache Content :";
    
    for(auto it = temp_cache.getCacheContent().begin(); it != temp_cache.getCacheContent().end(); ++it)
    {
        std::cout << " " << it->first;
    }

    std::cout << "\n";

    // 4. temp_cache 中是全部合并好的内容 广播给所有线程的 syn_cache
    auto temp_cache_content = temp_cache.getCacheContent();
    umit = _tid_cache_map.begin();
    while(umit != _tid_cache_map.end())
    {
        std::cout << "\t\tthread[ " << umit->first << " ] -> Update Cache Success...\n";

        lock_guard<mutex> cur_cache_syn_lock(umit->second->getCacheLock());

        auto & cur_cache_content = umit->second->getPendingList();    // 对于DoubleCache来说 PendingList 是 syn_cache 的内容

        cur_cache_content.clear();

        cur_cache_content.insert(cur_cache_content.end(), temp_cache_content.begin(), temp_cache_content.end());
        std::cout << "\t\tCur Cache Content :"; 

        for(auto clit = cur_cache_content.begin(); clit != cur_cache_content.end(); ++clit)
        {
            std::cout << " " << clit->first;
        }
        std::cout << "\n";

        umit->second->updataCache();

        ++umit;
    }
}
