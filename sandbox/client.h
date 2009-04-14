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

    inline bool is_idle_and_supports(const MethodDescriptor& method_name) const 
        { return (is_idle()) && (supports_method(method_name)); }
    
    virtual ~Client(){}
};

#endif