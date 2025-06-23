import faiss
import re
from sentence_transformers import SentenceTransformer
import xml.etree.ElementTree as ET

def read_webpage_lib() -> list:
    docs = []

    with open("../../../data/unrepeated_webpage.dat", "r", encoding="utf-8", errors="ignore") as f:
        text = f.read()

    print("Read Webpage OK...")

    webpage_list = re.findall(r'<webpage>.*?</webpage>', text, re.DOTALL)

    print("Begin XML Analysis...")

    for webpage in webpage_list:
        try:
            webpage = re.sub(r'&(?![a-zA-Z]+;)', '&amp;', webpage)

            element = ET.fromstring(webpage)

            content = element.findtext("content")

            if content:
                docs.append(content.strip())
        except ET.ParseError as e:
            continue


    print("Analysis OK...")

    return docs

def main():
    docs = read_webpage_lib()

    model = SentenceTransformer("./text2vec-base-chinese")

    docs_vectors = model.encode(docs, normalize_embeddings=True)

    print("Model Encode OK...")

    faiss_index = faiss.IndexFlatIP(docs_vectors.shape[1])
    faiss_index.add(docs_vectors)

    print("Create Faiss Index...")

    faiss.write_index(faiss_index, "webpage_faiss.index")

    return

if __name__ == "__main__":
    main()