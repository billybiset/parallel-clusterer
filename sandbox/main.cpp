#include "counter.h"

#include "process_client.h" /*the client part*/

#include <iostream>

#include "waiting.h"

#include <boost/thread.hpp>

void client_creator()
{
    ProcessClient pc; /*just create one client for testing, for now, nothing else will work*/
}

int main()
{
    boost::thread process_thread(client_creator);

    Counter c(158);
    c.run();

    c.wait_completion();
}
    
//     std::cout << "waiting join" << std::endl;
//     process_thread.join();
