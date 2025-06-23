#ifndef __SERVER_H__
#define __SERVER_H__

#include "utilities.h"
#include "recommend.h"
#include "websearch.h"
#include "semantic.h"
#include<workflow/WFFacilities.h>
#include<wfrest/HttpServer.h>

class NetServer
{
public:
    NetServer(int count);
    ~NetServer();
    void start();
    void stop();

private:
    void loadModules();
    void loadStaticResource();
    void keyWordRecommendMoudle();
    void webPageSearchMoudle();
    string uriDecode(const string & encode_str);

private:
    WFFacilities::WaitGroup _wait_group;
    wfrest::HttpServer _server;
};

#endif