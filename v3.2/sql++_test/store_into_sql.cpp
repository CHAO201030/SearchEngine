#include <cpp58.hpp>
#include <mysql++/mysql++.h>

void storeEnglishDictIndex()
{
    mysqlpp::Connection conn;

    conn.connect("SearchEngine", "192.168.239.128", "root", "1234", 3306);

    ifstream en_index_dict_ifs("/home/lhc/My_Code/SearchEngine/data/en_dict_index.dat");

    string line;

    double i = 0;

    while(getline(en_index_dict_ifs, line))
    {
        printf("\r%5.2lf%%", i++/26);
        fflush(stdout);

        istringstream iss(line);

        string word;

        int index = 0;

        iss >> word;

        while(iss >> index)
        {
            mysqlpp::Query query = conn.query();

            query << "INSERT INTO en_index_map(word, idx) VALUES (" << mysqlpp::quote << word << ", " << index << ")";

            query.exec();
        }
    }

    printf("\r100.00%%\n");

    en_index_dict_ifs.close();
}

void storeChineseDictIndex()
{
    mysqlpp::Connection conn;

    conn.connect("SearchEngine", "192.168.239.128", "root", "1234", 3306);

    ifstream zh_index_dict_ifs("/home/lhc/My_Code/SearchEngine/data/zh_dict_index.dat");

    string line;

    int i = 1;

    while(getline(zh_index_dict_ifs, line))
    {
        printf("\rcount : %d", i++);
        fflush(stdout);

        istringstream iss(line);

        string word;

        int index = 0;

        iss >> word;

        while(iss >> index)
        {
            mysqlpp::Query query = conn.query();

            query << "INSERT INTO zh_index_map(word, idx) VALUES (" << mysqlpp::quote << word << ", " << index << ")";

            query.exec();
        }
    }

    printf("\rtotal : %d\n", i);

    zh_index_dict_ifs.close();    
}

void storeEnglishDict()
{
    mysqlpp::Connection conn;

    conn.connect("SearchEngine", "192.168.239.128", "root", "1234", 3306);

    ifstream en_dict_ifs("/home/lhc/My_Code/SearchEngine/data/en_dict.dat");

    string line;

    int i = 0;

    while(getline(en_dict_ifs, line))
    {
        printf("\rcount : %d", i++);
        fflush(stdout);

        istringstream iss(line);

        string word;

        int frequence = 0;

        iss >> word >> frequence;

        mysqlpp::Query query = conn.query();

        query << "INSERT INTO en_dict(word, frequence) VALUES (" << mysqlpp::quote << word << ", " << frequence << ")";

        query.exec();
    }

    printf("\rtotal : %d\n", i);

    en_dict_ifs.close();
}

void storeChineseDict()
{
    mysqlpp::Connection conn;

    conn.connect("SearchEngine", "192.168.239.128", "root", "1234", 3306);

    ifstream zh_dict_ifs("/home/lhc/My_Code/SearchEngine/data/zh_dict.dat");

    string line;

    int i = 0;

    while(getline(zh_dict_ifs, line))
    {
        printf("\rcount : %d", i++);
        fflush(stdout);

        istringstream iss(line);

        string word;

        int frequence = 0;

        iss >> word >> frequence;

        mysqlpp::Query query = conn.query();

        query << "INSERT INTO zh_dict(word, frequence) VALUES (" << mysqlpp::quote << word << ", " << frequence << ")";

        query.exec();
    }

    printf("\rtotal : %d\n", i);

    zh_dict_ifs.close();
}

int main(void)
{
    // storeEnglishDictIndex();

    // storeChineseDictIndex();

    storeEnglishDict();

    storeChineseDict();

    return 0;
}
