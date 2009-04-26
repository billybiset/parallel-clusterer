#include <boost/thread.hpp>

#include "structures_db.h"

#include "representatives_job.h"

#include "clusterer_threads_client.h" /*the client part*/

using namespace parallel_clusterer;

void client_creator()
{
    new ClustererThreadsClient();
    new ClustererThreadsClient();
}

int main()
{
    boost::thread process_thread(client_creator);

    Structures_DB* db;
    db = new Structures_DB();

    RepresentativesJob * repjob;
    repjob = new RepresentativesJob(db);
    repjob->run();

    repjob->wait_completion();

//     wait(1000); //DISCUSS THIS, if not present, main may finish before some thread and destroy my Counter
//objects, by doing this some still executing thread can try to invoke methods on an object destroyed here!
//for design reasons, i can't do a distributer::get_instance.cleanup() or something, can I do a
//boost::join_all_threads() :P ?
}
