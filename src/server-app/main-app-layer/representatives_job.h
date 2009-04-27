#ifndef REPRESENTATIVES_JOB_H
#define REPRESENTATIVES_JOB_H

#include <list>

#include "structures_db.h"
#include "distributable_job.h"
#include "job_unit.h"
#include "common.h"

namespace parallel_clusterer
{

    class RepresentativesJob : public DistributableJob
    {
        public:
            RepresentativesJob(Structures_DB* sdb);

            virtual ~RepresentativesJob(){};

        private:
            virtual bool finished_generating() const;

            virtual JobUnit* get_next_job_unit(JobUnitSize size);

            class RepresentativesJobUnit : public JobUnit
            {
                public:
                    RepresentativesJobUnit(std::list<Structure *> marked_list, std::list<Structure *> unmarked_list);

                    virtual const char* method_name_required() const { return "reps";}

                    virtual ~RepresentativesJobUnit(){};
                private:
                    std::list<Structure *> _marked_list;
                    std::list<Structure *> _unmarked_list;
            };

            Structures_DB* _db;
    };
}

#endif