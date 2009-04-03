#include "counter.h"

#include "process_client.h" /*the client part*/

#include <iostream>

#include "waiting.h"

int main()
{

    int a = 0;

    if ( ! fork() )
    {
        while ( a == 0)
            ;
    }
    a = 1;

    ProcessClient pc;

    wait(1000);

    Counter c(128);
    c.run();

    wait(1000);
    ProcessClient pc1;

    wait(1000);

    c.wait_completion();

}