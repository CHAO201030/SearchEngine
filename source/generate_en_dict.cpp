#include "../include/Configuration.h"
#include "../include/SplitToolCppJieba.h"
#include "../include/DictProducer.h"

int main(void)
{
    string english_corpus_dir = Configuration::getInstance()->getConfig("english_corpus_dir");

    string english_stop_wrods_file = Configuration::getInstance()->getConfig("english_stop_words_file");

    DictProducer zh_dict(english_corpus_dir, english_stop_wrods_file, SE_LT_English);

    zh_dict.generateAllFiles();

    return 0;
}
