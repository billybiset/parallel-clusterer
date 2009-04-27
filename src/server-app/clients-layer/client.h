#ifndef CLIENT_H
#define CLIENT_H

#include <boost/thread.hpp>

#include "job_unit.h"
#include "common.h"

namespace parallel_clusterer
{
    class Client
    {
        public:
            virtual bool supports_method(const MethodDescriptor& method_name) const = 0;
            virtual bool process(const JobUnit* job_unit)                           = 0;

            enum Status {kBusy, kIdle};

            inline Status get_status() const
                { return _status;}

            virtual ~Client(){}

            inline bool is_idle_and_supports(const MethodDescriptor& method_name) const 
                { return _status == kIdle && supports_method(method_name); }

        protected:
            Client();
            void   set_status(Status new_status);

        private:
            Status       _status;
            boost::mutex _status_mutex;
            boost::mutex _busy_lock;
    };
}
#endif