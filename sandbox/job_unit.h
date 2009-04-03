#ifndef JOB_UNIT_H
#define JOB_UNIT_H

#include <cstring>

struct JobUnit
{
    virtual void print_info() const = 0;
    virtual const char * method_name_required() const = 0;
};

#endif