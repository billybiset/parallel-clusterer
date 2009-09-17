#ifndef DISTRIBUTABLE_JOB_H
#define DISTRIBUTABLE_JOB_H

#include <string>
#include <set>

#include <boost/thread.hpp>

#include "job_unit.h"
#include "common.h"
#include "scheduler_events.h"

namespace parallel_clusterer
{
    class DistributableJob;

    struct DistributableJobEventProducer
    {
        virtual void distributable_job_completed_event(DistributableJob* distob) = 0;
//         virtual void distributable_job_done_generating_event(DistributableJob* distob) = 0;
    };

    struct DistributableJobEventConsumer
    {
        virtual void handle_distributable_job_completed_event(DistributableJob* distjob)  = 0;
//         virtual void handle_distributable_job_done_generating_event(DistributableJob* distob) = 0;
    };

    class DistributableJobCompletedEvent : public Event
    {
        public:
            DistributableJobCompletedEvent(DistributableJobEventConsumer* interface,DistributableJob* distjob);
        private:
            virtual void call();
            DistributableJobEventConsumer* _interface;
            DistributableJob*              _distjob;
    };

/*
    class DistributableJobDoneGeneratingEvent : public Event
    {
        public:
            DistributableJobDoneGeneratingEvent(DistributableJobEventConsumer* interface,DistributableJob* distjob);
        private:
            virtual void call();
            DistributableJobEventConsumer* _interface;
            DistributableJob*              _distjob;
    };
*/

    class DistributableJob : public DistributableJobEventProducer, public Producer
    {
        public:
            /*interface for main*/
            void  run()                 const;
            void  wait_completion();
            void  inform_generation ();

            bool  completion_accepted(const JobUnitID& id);
            void set_listener(DistributableJobEventConsumer* const interface);

            virtual void      process_results (const JobUnitID id, const std::string* message) = 0;

            virtual JobUnit*  get_next_job_unit (JobUnitSize  size)        = 0;

            virtual bool      finished_generating()                  const = 0;

            virtual bool      ready_to_produce()                     const = 0;

            virtual const char* get_name()                           const = 0;

            virtual void      output_results()                       const = 0;

            virtual ~DistributableJob(){};

        protected:
            DistributableJob();
        private:
            void distributable_job_completed_event(DistributableJob* distob);
            void handle_distributable_job_done_generating_event(DistributableJob* distob);

            bool  finished();

            DistributableJobEventConsumer* _interface;

            std::set<JobUnitID> _completed;           /*when _completed.size() = _j_u_gen then */
            size_t              _job_units_generated; /*the job is completed.                  */

            boost::mutex              _completed_mutex;
            boost::mutex              _job_units_generated_mutex;
            boost::condition_variable _condition;
    };
}
#endif
