# Author : CHAO
# 当前目录所有文件生成一个可执行程序
SRCS_C := $(wildcard *.c)
SRCS_CC := $(wildcard *.cc)
SRCS_CPP := $(wildcard *.cpp)

OBJS := $(patsubst %.c, %.o, $(SRCS_C))
OBJS += $(patsubst %.cc, %.o, $(SRCS_CC))
OBJS += $(patsubst %.cpp, %.o, $(SRCS_CPP))

Out  := v3.2

CXX := g++
CXXFLAGS = -g -ltinyxml2 -lwfrest -lworkflow -lxapian -lmysqlpp -lmysqlclient -I/usr/include/mysql/

$(Out) : $(OBJS)
	$(CXX) $^ -o $@ $(CXXFLAGS)

%.o: %.c
	$(CXX) -c $< $(CXXFLAGS)

%.o: %.cc
	$(CXX) -c $< $(CXXFLAGS)

%.o: %.cpp
	$(CXX) -c $< $(CXXFLAGS)

.PHONY: clean rebuild

clean:
	$(RM) $(OBJS) $(Out)

rebuild: clean $(Out)