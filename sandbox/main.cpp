#include "counter.h"

#include <iostream>

int main()
{
    Counter c(128);
    c.run();
    std::cout << "I created " << &c << std::endl;
/*    while (! c.finished() )
    {
    }*/
}