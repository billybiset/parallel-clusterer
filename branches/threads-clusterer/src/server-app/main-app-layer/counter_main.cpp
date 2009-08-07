/*
  Parallel Clusterer: A distributed protein backbone clusterer.
    Copyright (C) 2009 Guillermo Biset, FuDePAN
    
    This file is part of the Parallel Clusterer Project.

    File:           clusterer_threads.cpp
    Contents:       Main application.

    System:         Parallel Clusterer
    Language:       C++
  
    Author:         Guillermo Biset
    E-Mail:         billybiset AT gmail.com
  
    Parallel Clusterer is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    Parallel Clusterer is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with Parallel Clusterer.  If not, see <http://www.gnu.org/licenses/>.
*/

#include <syslog.h>
#include <sys/time.h>

#include "appjobs/counter.h"

using namespace parallel_clusterer;

int main()
{
    timeval tm;
    gettimeofday(&tm, NULL);

    openlog ("FuDP",NULL, LOG_LOCAL1);
    syslog(LOG_NOTICE,"Start.");

    Counter * counter_job1 = new Counter("data/file2.txt");
    Counter * counter_job2 = new Counter("data/short.txt");
    Counter * counter_job3 = new Counter("data/file1.txt");
    Counter * counter_job4 = new Counter("data/file2.txt");

    counter_job1->run();
    counter_job2->run();
    counter_job3->run();
    counter_job4->run();

    counter_job1->wait_completion();
    counter_job2->wait_completion();
    counter_job3->wait_completion();
    counter_job4->wait_completion();

    counter_job1->output_results();
    counter_job2->output_results();
    counter_job3->output_results();
    counter_job4->output_results();
}
