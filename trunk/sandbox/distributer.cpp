#include "distributer.h"

#include <iostream>

Distributer* Distributer::_instance = 0; // initialize pointer

Distributer* Distributer::get_instance () 
{
    if (_instance == 0)  // is it the first call?
    {  
        _instance = new Distributer; // create sole instance
    }
    return _instance; // address of sole instance
}

Distributer::Distributer() 
   : _distJobs(1) // eventually bigger :)
//     _jobQueue(11);
//     _pendingList(11);    
{
    _status = kStopped;
}

DistributableJob* Distributer::jobs_available()
{
    return _distJobs[0]; //BAD: reallly!
}

void Distributer::start_scheduler()
{
    if (! fork())
    {
        _status = kRunning;
        while (_status != kStopped)
        {
            DistributableJob* job;
            if (job = jobs_available())
            {
                JobUnit* job_unit;
                if ( job_unit = job->get_next_job_unit(10) )
                {
                    //find client, if you have one, try to assign the job unit. But for now:
                    job_unit->print_info();
                }
                else
                {
                    std::cout << "The DistJob " << job << " has no more job units." << std::endl;
                    _status = kStopped; //obviously this will be changed
                }
            }
            else
            {
                std::cout << "no distributable jobs available." << std::endl;
                _status = kStopped; //obviously this will be changed
            }
        }
    }
}

void Distributer::enqueue(DistributableJob* const distjob)
{
    _distJobs[0] = distjob;
}


