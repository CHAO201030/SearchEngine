#include "../include/Configuration.h"
#include "../include/DirScanner.h"
#include "../include/WebPageLib.h"

int main(void)
{
    WebPageLib wpl;

    wpl.create();

    wpl.store();

    return 0;
}
