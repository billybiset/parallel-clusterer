#include <boost/thread.hpp>

#include "structures_db.h"

#include "representatives_job.h"

// #include "clusterer_threads_client.h" /*the client part, this won't go in the boinc-clusterer*/

using namespace parallel_clusterer;

// void client_creator()
// {
//     new ClustererThreadsClient();
//     new ClustererThreadsClient();
// }

int main()
{
//     boost::thread process_thread(client_creator);

    Structures_DB* db;
    db = new Structures_DB();

    RepresentativesJob * repjob;
    repjob = new RepresentativesJob(db);
    repjob->run();

    repjob->wait_completion();

}
