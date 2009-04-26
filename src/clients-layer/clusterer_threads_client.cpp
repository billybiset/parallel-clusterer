#include <iostream> //ridiculous

#include <string>

#include <boost/thread/mutex.hpp>

#include "clusterer_threads_client.h"
#include "threads_clients_manager.h"
#include "representatives_job.h"

using namespace parallel_clusterer;

ClustererThreadsClient::ClustererThreadsClient() :
    Client(),
    _processing_lock()
{
   ThreadsClientsManager::get_instance()->register_client(this);
}

bool ClustererThreadsClient::supports_method(const MethodDescriptor& method_name) const
{
//     return ! method_name.compare("count");
    return true; // :P
}

bool ClustererThreadsClient::process(const JobUnit* job_unit)
{
    //assert job_unit->required_method_name == "count"

    boost::mutex::scoped_lock glock(_processing_lock);
    set_status(kBusy);

    /*dispatch*/

    set_status(kIdle);
    ThreadsClientsManager::get_instance()->inform_completion(job_unit); //->get_id()

    return        false;
}