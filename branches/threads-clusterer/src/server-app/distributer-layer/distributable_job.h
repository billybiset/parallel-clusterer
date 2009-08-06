#ifndef DISTRIBUTABLE_JOB_H
#define DISTRIBUTABLE_JOB_H

#include <string>
#include <set>

#include <boost/thread.hpp>

#include "job_unit.h"
#include "common.h"

namespace parallel_clusterer
{

    class DistributableJob
    {
        public:
            /*interface for main*/
            void  run()              const;
            void  wait_completion();
            void  inform_generation ();

            bool  finished();
            bool  completion_accepted(const JobUnitID& id);

            virtual void      process_results (JobUnitID id, const std::string& message) = 0;

            virtual JobUnit*  get_next_job_unit (JobUnitSize  size)        = 0;

            virtual bool      finished_generating()                  const = 0;

            virtual const char* get_name()                           const = 0;

            virtual void      output_results()                       const = 0;

            virtual ~DistributableJob(){};

        protected:
            DistributableJob();
        private:
            std::set<JobUnitID> _completed;

            size_t _job_units_generated;

            boost::mutex _completed_mutex;
            boost::mutex _job_units_generated_mutex;
    };
}
#endif
