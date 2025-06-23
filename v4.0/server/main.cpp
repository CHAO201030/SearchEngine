#include "./include/utilities.h"
#include "./include/server.h"

int main()
{
    NetServer server(1);

    server.start();

    return 0;
}