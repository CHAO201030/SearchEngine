#include "./include/server.h"

NetServer::NetServer(int count):_wait_group(count)
{
    Dictionary::getInstance();
    loadModules();
}

NetServer::~NetServer()
{
    Dictionary::delInstance();
}

void NetServer::start()
{
    int port = atoi(Configuration::getInstance()->getConfig("port").c_str());

    if(_server.track().start(port) == 0)
    {
        // _server.list_routes();
        _wait_group.wait();
        _server.stop();
    }
    else
    {
        std::cerr << "[ERROR] : HTTP Server Start Failed...\n";
        exit(-1);
    }
}

void NetServer::stop()
{
    _wait_group.done();
}

void NetServer::loadModules()
{
    loadStaticResource();
    keyWordRecommendMoudle();
    webPageSearchMoudle();
}

void NetServer::loadStaticResource()
{
    _server.GET(
        "/", 
        [](const wfrest::HttpReq * req, wfrest::HttpResp * resp)
        {
            resp->File(Configuration::getInstance()->getConfig("static_resource"));
        }
    );
}

void NetServer::keyWordRecommendMoudle()
{
    _server.GET(
        "/recommend",
        [this](const wfrest::HttpReq *req, wfrest::HttpResp * resp, SeriesWork * series)
        {
            string encode_uri = req->query("query");
            string query_word = uriDecode(encode_uri);

            if(query_word.empty())
            {
                resp->String("No Result");
            }
            else
            {
                KeyRecommend tool;

                string result = tool.recommend(query_word);
                resp->String(result);
            }
        }
    );
}

void NetServer::webPageSearchMoudle()
{
    _server.GET(
        "/search",
        [this](const wfrest::HttpReq *req, wfrest::HttpResp * resp, SeriesWork * series)
        {
            string encode_uri = req->query("query");
            string query_word = uriDecode(encode_uri);

            if(query_word.empty())
            {
                resp->String("No Result");
            }
            else
            {
                WebSearch tool;

                // string traditional_result = tool.search(query_word);
                
                PyInferenceTool py_tool;

                vector<int> doc_id_vec = py_tool.inference(query_word);

                string ai_result = tool.search(doc_id_vec);

                resp->String(ai_result);
            }
        }
    );
}

string NetServer::uriDecode(const string & encode_str)
{
    std::ostringstream decode_oss;
    size_t len = encode_str.length();

    for(size_t i = 0; i < len; i++)
    {
        if (encode_str[i] == '%' && i + 2 < len)
        {
            std::string hex = encode_str.substr(i + 1, 2);
            char decodedChar = static_cast<char>(std::stoi(hex, nullptr, 16));
            decode_oss << decodedChar;
            i += 2;
        }
        else if (encode_str[i] == '+')
        {
            decode_oss << ' ';
        }
        else
        {
            decode_oss << encode_str[i];
        }
    }

    return decode_oss.str();
}