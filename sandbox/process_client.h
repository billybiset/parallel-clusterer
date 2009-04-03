#ifndef PROCESS_CLIENT_H
#define PROCESS_CLIENT_H

#include <cstring>

#include "client.h"

enum ClientStatus {kBusy, kIdle};

class ProcessClient : Client
{
    public:
        ProcessClient();

        virtual bool is_idle() const;
        virtual bool is_busy() const;

        virtual bool supports_method(const char * const method_name) const;
        virtual bool process(const JobUnit* const job_unit, const void * return_data);

    private:
        void    count(unsigned int start, unsigned int amount);

        enum    ClientStatus _status;
};

#endif