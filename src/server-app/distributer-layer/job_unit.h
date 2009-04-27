#ifndef JOB_UNIT_H
#define JOB_UNIT_H

#include "common.h"

namespace parallel_clusterer
{
    class JobUnit
    {
        public:
            virtual const char* method_name_required() const = 0;

            inline  JobUnitID   get_id()               const {return _id;}

        protected:
            JobUnit();          
            virtual ~JobUnit(){};

        private:
            static JobUnitID _last_generated;
            JobUnitID        _id;
    };
}

#endif