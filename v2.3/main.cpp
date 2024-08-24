#include "../include/SearchEngineServer.h"
#include "../include/CacheManager.h"

CacheManager server_cache_manager;

int cache_syn_time = std::stoi(Configuration::getInstance()->getConfig("CacheSynTime"));

void timerFunc(WFTimerTask * timer_task)
{
    WFTimerTask * task  = WFTaskFactory::create_timer_task(cache_syn_time, 0, timerFunc);

    server_cache_manager.updateAllThreadCache();

    series_of(timer_task)->push_back(task);
}

void setTimer()
{
    WFTimerTask * timer_task = WFTaskFactory::create_timer_task(cache_syn_time, 0, timerFunc);

    timer_task->start();
}

void test()
{
    SearchEngineServer server(1);

    setTimer();

    server.start();
}

int main(void)
{
    test();

    return 0;
}
