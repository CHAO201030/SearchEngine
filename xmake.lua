-- 设置编译器及编译选项
set_toolchains("gcc")
add_requires("tinyxml2", "workflow", "wfrest", "xapian")

-- 添加编译选项
add_cxxflags("-g")

set_kind("binary")

set_targetdir("bin")

-- /************************************************** 离线阶段 **************************************************/

-- 生成字典

target("generate_en_dict")
    add_files("source/Configuration.cpp", "source/SplitToolCppJieba.cpp", "source/DictProducer.cpp", "source/generate_en_dict.cpp")

target("generate_zh_dictt")
    add_files("source/Configuration.cpp", "source/SplitToolCppJieba.cpp", "source/DictProducer.cpp", "source/generate_zh_dict.cpp")


-- 生成网页库和网页偏移库

target("generate_web_page_lib")
    add_files("source/Configuration.cpp", "source/DirScanner.cpp", "source/WebPageLib.cpp", "source/generate_web_page_lib.cpp")
    add_packages("tinyxml2")


-- 生成去重网页库和去重网页偏移库

target("generate_unrepeated_web_page_lib")
    add_files("source/Configuration.cpp", "source/PageLibPreprocessor.cpp", "source/WebPage.cpp", "source/SplitToolCppJieba.cpp", "source/generate_unrepeated_web_page_lib.cpp")
    add_packages("tinyxml2")


-- 生成去重网页倒排索引库

target("generate_invert_index_lib")
    add_files("source/Configuration.cpp", "source/PageLibPreprocessor.cpp", "source/WebPage.cpp", "source/SplitToolCppJieba.cpp", "source/generate_invert_index_lib.cpp")
    add_packages("tinyxml2")


-- /************************************************** 在线阶段 **************************************************/

target("v2.1")
    add_files("v2.1/*.cpp")
    add_packages("tinyxml2", "wfrest", "workflow")

target("v2.2")
    add_files("v2.2/*.cpp")
    add_packages("tinyxml2", "wfrest", "workflow")

target("v2.3")
    add_files("v2.3/*.cpp")
    add_packages("tinyxml2", "wfrest", "workflow")

target("v2.4")
    add_files("v2.4/*.cpp")
    add_packages("tinyxml2", "wfrest", "workflow")

target("v3.1")
    add_files("v3.1/*.cpp")
    add_packages("tinyxml2", "wfrest", "workflow", "xapian")


-- -- 定义目标：produce_index
-- target("produce_index")
--     add_files("src/common/*.cc", "src/dict_offline/produce_index.cc", "src/dict_offline/dic_producer.cc")

-- -- 定义目标：produce_weblib_raw
-- target("produce_weblib_raw")
--     add_files("src/common/*.cc", "src/webpage_offline/file_processor.cc", "src/webpage_offline/page_lib.cc", "src/webpage_offline/produce_wbpg_raw.cc")
--     add_packages("tinyxml2")

-- -- 定义目标：produce_weblib
-- target("produce_weblib")
--     add_files("src/common/*.cc", "src/webpage_offline/page_lib_preprocessor.cc", "src/webpage_offline/produce_wbpg.cc")
--     add_packages("tinyxml2")

-- -- 定义目标：server
-- target("server")
--     add_files("src/common/*.cc", "src/key_recommend/dictionary.cc", "src/key_recommend/key_recommander.cc", "src/server/*.cc", "src/webpage_query/webpage_query.cc", "src/main.cc")
--     add_packages("tinyxml2", "boost_system")

-- -- 定义目标：test
-- target("test")
--     add_files("src/test.cc")