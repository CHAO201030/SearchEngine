#include <xapian.h>
#include <cpp58.hpp>

using namespace Xapian;



void test1(char * s)
{
    WritableDatabase write_db("./test2", DB_OPEN);

    Enquire enquire(write_db);

    Query q(s);

    cout << "Query : " << q.get_description() << "\n";

    enquire.set_query(q);

    MSet m = enquire.get_mset(0, 10);

    cout << m.get_matches_estimated() << " find\n";
    cout << "size : 1 -> " << m.size() << "\n";

    for(MSetIterator mit = m.begin(); mit != m.end(); ++mit)
    {
        Document doc = mit.get_document();

        string id = doc.get_value(0);
        string title = doc.get_value(1);
        string url = doc.get_value(2);

        cout << "doc_id = " << id << "\n"
             << "title  = " << title << "\n"
             << "url    = " << url << "\n"
             << "\t" << doc.get_data() << "\n";
    }
}

int main(int argc, char ** argv)
{
    if(argc < 2)
    {
        cout << "fuck u\n";

        exit(-1);
    }

    test1(argv[1]);

    return 0;
}