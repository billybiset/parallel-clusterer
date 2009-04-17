#ifndef PROCESS_CLIENT_H
#define PROCESS_CLIENT_H

#include <string>

#include <boost/thread/mutex.hpp>

#include "client.h"

class ProcessClient : Client
{
    public:
        ProcessClient();

        virtual bool is_idle() const;
        virtual bool is_busy() const;

        virtual bool supports_method(const MethodDescriptor& method_name) const;
        virtual bool process(const JobUnit* const job_unit, const void * return_data);

    private:

        enum Status {kBusy, kIdle};

        void set_status(const enum Status new_status);

        void count(unsigned int start, unsigned int amount);

        enum Status _status;
        boost::mutex      _status_mutex;
};

#endif