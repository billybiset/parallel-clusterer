#ifndef CLIENT_H
#define CLIENT_H

#include "job_unit.h"
#include "common.h"

using namespace parallel_clusterer;

struct Client
{
    virtual bool is_idle() const = 0;
    virtual bool is_busy() const = 0;

    virtual bool supports_method(const MethodDescriptor& method_name)     const = 0;
    virtual bool process(const JobUnit* job_unit,const void* return_data)       = 0;

//     virtual ~Client();
};

#endif