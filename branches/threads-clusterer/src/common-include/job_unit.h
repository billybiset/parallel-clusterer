#ifndef JOB_UNIT_H
#define JOB_UNIT_H

#include <string>

#include "common.h"
#include "serializable_object.h"

namespace parallel_clusterer
{
    class JobUnit : public SerializableObject
    {
        public:
            virtual const char*         method_name_required() const = 0;

            virtual const std::string   serialize()            const;

            inline  JobUnitID   get_id()               const {return _id;}

            inline  JobUnitSize get_size()             const {return _size;}

        protected:
            JobUnit();
            virtual ~JobUnit(){};

            void  set_size(JobUnitSize size);

        private:
            virtual const std::string&  get_message()          const = 0;

            static JobUnitID _last_generated;
            JobUnitID        _id;
            JobUnitSize      _size;
    };
}

#endif