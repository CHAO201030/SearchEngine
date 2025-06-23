#include "./include/semantic.h"

PyInferenceTool::PyInferenceTool()
{
    string ip = Configuration::getInstance()->getConfig("python_server_ip");
    string port = Configuration::getInstance()->getConfig("python_server_port");
    string addr = ip + ":" + port;
    shared_ptr<Channel> py_io_channel = grpc::CreateChannel(addr, grpc::InsecureChannelCredentials());
    _stub = std::move(SemanticSearch::NewStub(py_io_channel));
}

PyInferenceTool::PyInferenceTool(shared_ptr<Channel> channel):_stub(SemanticSearch::NewStub(channel))
{

}

vector<int> PyInferenceTool::inference(const string & query)
{
    pyInferenceReq req;
    pyInferenceResp resp;

    req.set_query(query);

    ClientContext ctx;

    if(_stub->inference(&ctx, req, &resp).ok())
    {
        return vector<int> (resp.doc_id().begin(), resp.doc_id().end());
    }
    else
    {
        return {};
    }
}