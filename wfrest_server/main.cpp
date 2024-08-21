#include "../include/SearchEngineServer.h"

int main(void)
{
    // SearchEngineServer se_ser(1);
    NetServer server(1);
    
    server.loadModules();

    server.start();

    return 0;
}
