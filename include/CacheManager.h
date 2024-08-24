#ifndef __CACHE_MANAGER_H__
#define __CACHE_MANAGER_H__

#include "SynCacge.h"
#include "Configuration.h"
#include "DoubleCache.h"

class CacheManager
{
public:
    Cache * getThreadCache(pid_t thread_id);

public:
    void updateAllThreadCache();
    void generateOperateSequence(list<pair<string, string>> & operator_sequence_list);
    void updateAllThreadDoubleCache();
private:
    unordered_map<pid_t, Cache *> _tid_cache_map;

};

#endif
