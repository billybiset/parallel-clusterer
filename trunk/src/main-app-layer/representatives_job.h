#ifndef COUNTER_H
#define COUNTER_H

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
                    RepresentativesJobUnit();
                    virtual const char* method_name_required() const { return "count";}

                    virtual ~RepresentativesJobUnit(){};
                private:
            };

            Structures_DB* _structures_db;
    };
}

#endif