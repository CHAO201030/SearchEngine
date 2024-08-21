#ifndef __NET_SERVER_H__
#define __NET_SERVER_H__

#include "Configuration.h"
#include "nlohmann/json.hpp"
#include "SplitToolCppJieba.h"
#include "KeyRecommend.h"
#include "WebSearch.h"
#include "Dictionary.h"

#include <workflow/WFFacilities.h>
#include <wfrest/HttpServer.h>
#include <iostream>
#include <vector>
#include <string>
#include <map>
#include <unordered_map>
#include <set>
#include <queue>
#include <utility>
#include <sys/time.h>
#include <sys/types.h>
#include <math.h>

using std::vector;
using std::pair;
using std::set;
using std::map;
using std::string;
using std::cout;
using std::cerr;
using std::priority_queue;
using std::unordered_map;
using namespace wfrest;

class NetServer
{
public:
    NetServer(int count);

    ~NetServer();

    void start();

    void stop();

    void loadModules();

private:
    void loadStaticResourceMoudle();

    void keyWordRecommendMoudle();

    void webPageSearchMoudle();

    string uriDecode(const string & encode_str);

private:
    WFFacilities::WaitGroup _wait_group;    // 等待组

    wfrest::HttpServer _server; // workflow异步HTTP服务器
};

#endif
