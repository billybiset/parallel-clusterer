#include "counter.h"

#include "process_client.h" /*the client part*/

#include <iostream>

#include "waiting.h"

#include <boost/thread.hpp>

void client_creator()
{
    new ProcessClient();
    new ProcessClient();
}

int main()
{
    boost::thread process_thread(client_creator);

    Counter c(158);
    c.run();

    c.wait_completion();
}
