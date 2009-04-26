#ifndef CLUSTERER_THREADS_CLIENT_H
#define CLUSTERER_THREADS_CLIENT_H

#include <string>

#include <boost/thread/mutex.hpp>

#include "client.h"

namespace parallel_clusterer
{
    class ClustererThreadsClient : Client
    {
        public:
            ClustererThreadsClient();

        private:
            virtual bool supports_method(const MethodDescriptor& method_name) const;
            virtual bool process(const JobUnit* const job_unit);

            boost::mutex _processing_lock;
    };
}

#endif