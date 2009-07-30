#ifndef COUNTER_H
#define COUNTER_H

#include <list>
#include <fstream>
#include <string>

#include "distributable_job.h"
#include "job_unit.h"

namespace parallel_clusterer
{

    class Counter : public DistributableJob
    {
        public:
            Counter(char* file_name);

            virtual void        output_results()      const;

            virtual ~Counter(){};
        private:
            virtual void        process_results (JobUnitID id, const std::string& message);

            virtual bool        finished_generating() const;
            virtual const char* get_name()            const;

            virtual JobUnit*    get_next_job_unit(JobUnitSize size);

            class CounterJobUnit : public JobUnit
            {
                public:
                    CounterJobUnit(char* message,JobUnitSize size);

                private:
                    virtual const char*  method_name_required() const { return "count";}
                    virtual ~CounterJobUnit(){};

                    virtual const std::string& get_message()       const;

                    std::string _message;
            };

            size_t       _total_count;
            std::fstream _file;
    };
}

#endif