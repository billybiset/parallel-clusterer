#ifndef CLIENT_H
#define CLIENT_H

#include <cstring>

#include "job_unit.h"

struct Client
{
    virtual bool is_idle() const = 0;
    virtual bool is_busy() const = 0;

    virtual bool supports_method(const char * const method_name) const = 0;
    virtual bool process(const JobUnit* const job_unit,const void* return_data) = 0;
};

#endif