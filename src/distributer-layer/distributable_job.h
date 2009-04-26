#ifndef DISTRIBUTABLE_JOB_H
#define DISTRIBUTABLE_JOB_H

#include "job_unit.h"

#include "common.h"

namespace parallel_clusterer
{

//     class DistJobClientAPI
//     {
        /*interface for JobUnit*/
//         public:
//             virtual void inform_completion (JobUnitID    job_unit_id) = 0;
//     };

    class DistributableJob  //: private DistJobClientAPI
    {
        public:
            /*interface for main*/
            void  run()              const;
            void  wait_completion()  const;

            bool  finished()         const;

            /*interface for layer2 (distributer), through vtable*/

            void inform_completion (JobUnitID    job_unit_id);
            void inform_generation ();

            virtual JobUnit*  get_next_job_unit (JobUnitSize  size)        = 0;
            

            virtual bool      finished_generating()                  const = 0;

            virtual ~DistributableJob(){};

        protected:
            DistributableJob();

            size_t _job_units_generated;
            size_t _job_units_completed;
    };
}
#endif
