#include "../include/NetServer.h"
#include "../include/CacheManager.h"
#include "../include/BigCache.h"
#include "../include/SynCacge.h"

extern CacheManager server_cache_manager;

NetServer::NetServer(int count):_wait_group(count)
{
    Dictionary::getInstance();
}

NetServer::~NetServer()
{

}

void NetServer::start()
{
    int port = atoi(Configuration::getInstance()->getConfig("PORT").c_str());
    if(_server.track().start(port) == 0)
    {
        _server.list_routes();
        _wait_group.wait();
        _server.stop();
    }
    else
    {
        cerr << "[ERROR] : HTTP Server Satrt Failed...\n";
        exit(-1);
    }
}

void NetServer::stop()
{
    _wait_group.done();
}

void NetServer::loadModules()
{
    loadStaticResourceMoudle();
    keyWordRecommendMoudle();
    webPageSearchMoudle();
}

void NetServer::loadStaticResourceMoudle()
{
    _server.GET("/", 
    
    [](const HttpReq * req, HttpResp * resp)
    {
        resp->File(Configuration::getInstance()->getConfig("static_resource"));
        // resp->File("static/favicon.ico");
    }
    );
}

void NetServer::keyWordRecommendMoudle()
{
    _server.GET("/recommend", 

    [this](const HttpReq * req, HttpResp * resp, SeriesWork * series)
    {
        string encode_uri = req->query("query");
        string query_word = uriDecode(encode_uri);

        if(query_word.empty())
        {
            resp->String("No query result");
        }
        else
        {
            KeyRecommend recommend;

            recommend.doRecommend(query_word);

            string res = recommend.getResult();

            resp->String(res);
        }
    });
}

void NetServer::webPageSearchMoudle()
{
    _server.GET("/search",

    [this](const HttpReq * req, HttpResp * resp, SeriesWork * series)
    {
        string encode_uri = req->query("query");
        string query_word = uriDecode(encode_uri);

        if(query_word.empty())
        {
            resp->String("No query result");
        }
        else
        {
            cout << "Thread -> " << gettid() << " do Search Task\n";

            Cache * cur_thread_cache = server_cache_manager.getThreadCache(gettid());

            string value = cur_thread_cache->getKV(query_word).second;

            if(value.empty())
            {
                cout << "[INFO] : Cache Missing... -> query_word = " << query_word << "\n";

                WebSearch search;

                search.doSearch(query_word);

                string res = search.getResult();

                cur_thread_cache->setKV(query_word, res);

                resp->String(res);
            }
            else
            {
                cout << "[INFO] : Cache OK... -> query_word = " << query_word << "\n";

                resp->String(value);
            }
        }
    });
}

string NetServer::uriDecode(const string & encodedURI)
{
    std::ostringstream decoded;
    size_t len = encodedURI.length();

    for (size_t i = 0; i < len; ++i)
    {
        if (encodedURI[i] == '%' && i + 2 < len)
        {
            std::string hex = encodedURI.substr(i + 1, 2);
            char decodedChar = static_cast<char>(std::stoi(hex, nullptr, 16));
            decoded << decodedChar;
            i += 2;
        }
        else if (encodedURI[i] == '+')
        {
            decoded << ' ';
        }
        else
        {
            decoded << encodedURI[i];
        }
    }

    return decoded.str();
}
