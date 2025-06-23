import faiss
import numpy as np
from sentence_transformers import SentenceTransformer

import grpc
import sementic_search_pb2
import sementic_search_pb2_grpc
from concurrent import futures

class InferenceModel:
    def __init__(self):
        self.model = SentenceTransformer("./text2vec-base-chinese")
        self.faiss_index = faiss.read_index("webpage_faiss.index")
    
    def do_inference(self, query, top_k=5):
        ret = []
        query_vec = self.model.encode(query, normalize_embeddings=True)
        similarity, doc_id = self.faiss_index.search(query_vec, top_k)

        for i in doc_id[0]:
            ret.append(i)

        return ret


class SementicSearch(sementic_search_pb2_grpc.SemanticSearchServicer):
    def __init__(self, model):
        super().__init__()
        self.model = model

    def inference(self, request, context):
        print(f"Get Query Word : {request.query}")

        query = [request.query]
        doc_id_list = self.model.do_inference(query)

        resp = sementic_search_pb2.pyInferenceResp()
        resp.doc_id.extend(doc_id_list)

        return resp


def main():
    model = InferenceModel()
    server = grpc.server(futures.ThreadPoolExecutor(max_workers=2))
    sementic_search_pb2_grpc.add_SemanticSearchServicer_to_server(SementicSearch(model), server)
    server.add_insecure_port("192.168.239.128:9527")
    print(f"Server Start...")
    server.start()
    server.wait_for_termination()

    return 0


if __name__ == "__main__":
    main()