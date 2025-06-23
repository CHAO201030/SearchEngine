#ifndef __SEMANTIC_H__
#define __SEMANTIC_H__

#include<iostream>
#include<vector>
#include<string>
#include<memory>
#include<grpcpp/grpcpp.h>
#include "utilities.h"
#include "semantic_search.grpc.pb.h"

using std::vector;
using std::string;
using std::unique_ptr;
using std::shared_ptr;

using grpc::Channel;
using grpc::ClientContext;
using grpc::Status;

class PyInferenceTool
{
public:
    PyInferenceTool();
    PyInferenceTool(shared_ptr<Channel> channel);

    vector<int> inference(const string & query);

private:
    unique_ptr<SemanticSearch::Stub> _stub; 
};

#endif