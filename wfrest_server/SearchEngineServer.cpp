#include "../include/SearchEngineServer.h"

SearchEngineServer::SearchEngineServer(int count):_server(count)
{

}

void SearchEngineServer::start()
{
    _server.loadModules();
    _server.start();
}

void SearchEngineServer::stop()
{
    _server.stop();
}
