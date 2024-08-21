#ifndef __SEARCH_ENGINE_SERVER_H__
#define __SEARCH_ENGINE_SERVER_H__

#include "NetServer.h"

class SearchEngineServer
{
public:
    SearchEngineServer(int count);
    
    void start();

    void stop();

private:
    NetServer _server;
};

#endif
