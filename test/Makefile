BIN_DIR := ../bin/

CXX := g++
CXXFLAGS = -g -std=c++11 -ltinyxml2

# 离线部分模块一 生成字典和字典索引文件

SRCS_DictProducer := DictProducer.cpp SplitToolCppJieba.cpp

OBJS_DictProducer := $(patsubst %.cpp, $(BIN_DIR)%.o, $(SRCS_DictProducer))

OUT_DictProducer := $(BIN_DIR)DictProducer

$(OUT_DictProducer) : $(OBJS_DictProducer)
	$(CXX) $^ -o $@ $(CXXFLAGS)

$(OBJS_DictProducer) : $(BIN_DIR)%.o: %.cpp
	$(CXX) -c $< -o $@ $(CXXFLAGS)


# 离线部分模块二 生成网页库和网页索引库



.PHONY: clean rebuild

clean:
	$(RM) $(OBJS_DictProducer) $(OUT_DictProducer)
