#ifndef PROCESS_CLIENT_H
#define PROCESS_CLIENT_H

#include <string>

#include "client.h"

#include <boost/thread/mutex.hpp>

enum ClientStatus {kBusy, kIdle};

class ProcessClient : Client
{
    public:
        ProcessClient();

        virtual bool is_idle() const;
        virtual bool is_busy() const;

        virtual bool supports_method(const MethodDescriptor& method_name) const;
        virtual bool process(const JobUnit* const job_unit, const void * return_data);

    private:

        void set_status(const enum ClientStatus new_status);

        enum ClientStatus get_status() const;

        void count(unsigned int start, unsigned int amount);

        enum ClientStatus _status;
        boost::mutex      _status_mutex;
};

#endif