#ifndef JOB_UNIT_H
#define JOB_UNIT_H

#include "common.h"

using namespace parallel_clusterer;

class JobUnit
{
    public:
        virtual       void               print_info()           const = 0;
        virtual const char*              method_name_required() const = 0;

        JobUnitID            get_id()               const;
    protected:
        JobUnit();
    private:
        static JobUnitID _last_generated;
        JobUnitID        _id;
};

#endif