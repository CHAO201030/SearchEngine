#include "../include/NetServer.h"

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
            WFRedisTask * redis_task = WFTaskFactory::create_redis_task("redis://127.0.0.1:6379", 10,
            
            [resp, query_word, series](WFRedisTask * redis_task)
            {
                protocol::RedisResponse * redis_resp = redis_task->get_resp();
                protocol::RedisValue redis_result;

                redis_resp->get_result(redis_result);

                if(redis_result.is_error() || redis_task->get_state() != WFT_STATE_SUCCESS)
                {
                    resp->String("Server Redis Error");
                }
                else
                {
                    string res = redis_result.string_value();

                    if(res.empty())
                    {
                        KeyRecommend recommend;

                        recommend.doRecommend(query_word);

                        res = recommend.getResult();

                        WFRedisTask * insert_task = WFTaskFactory::create_redis_task("redis://127.0.0.1:6379", 10, nullptr);
                        
                        cout << "\n[INFO] : Key Recommend Cache Missing...\n";
                        
                        insert_task->get_req()->set_request("SET", {query_word, res});
                        
                        series->push_back(insert_task);
                    }
                    
                    cout << "\n[INFO] : Key Recommend Cache OK...\n";

                    resp->String(res);
                }
            }
            );
 
            redis_task->get_req()->set_request("GET", {query_word});
            series->push_back(redis_task);

            // KeyRecommend recommend;

            // recommend.doRecommend(query_word);

            // string res = recommend.getResult();

            // resp->String(res);
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
            WFRedisTask * redis_task = WFTaskFactory::create_redis_task("redis://127.0.0.1:6379/1", 10,
            
            [resp, query_word, series](WFRedisTask * redis_task)
            {
                protocol::RedisResponse * redis_resp = redis_task->get_resp();
                protocol::RedisValue redis_result;

                redis_resp->get_result(redis_result);

                if(redis_result.is_error() || redis_task->get_state() != WFT_STATE_SUCCESS)
                {
                    resp->String("Server Redis Error");
                }
                else
                {
                    string res = redis_result.string_value();

                    if(res.empty())
                    {
                        WebSearch search;

                        search.doSearch(query_word);
                        
                        string res = search.getResult();

                        resp->String(res);

                        WFRedisTask * insert_task = WFTaskFactory::create_redis_task("redis://127.0.0.1:6379/1", 10, nullptr);

                        cout << "\n[INFO] : Web Search Cache Missing...\n";

                        insert_task->get_req()->set_request("SET", {query_word, res});
                        
                        series->push_back(insert_task);
                    }

                    cout << "\n[INFO] : Web Search Cache OK...\n";

                    resp->String(res);
                }
            }
            );
 
            redis_task->get_req()->set_request("GET", {query_word});
            series->push_back(redis_task);

            // WebSearch search;

            // search.doSearch(query_word);
            
            // string res = search.getResult();

            // resp->String(res);
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
