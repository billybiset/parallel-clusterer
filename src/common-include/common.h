/*
  Parallel Clusterer: A distributed protein backbone clusterer.
    Copyright (C) 2009 Guillermo Biset, FuDePAN
    
    This file is part of the Parallel Clusterer Project.

    File:           common.h
    Contents:       Header file for Parallel Cluster providing the main type
                    definitions.

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

#ifndef COMMON_H
#define COMMON_H

#include <stdlib.h>
#include <string>
#include <vector>

#include "string_message.h"

namespace parallel_clusterer
{
    /** ID of a JobUnit, should be unique. */
    typedef size_t       JobUnitID;


    /** ID of a Client, should be unique for each connected client thoroughout the programs lifetime. */
    typedef size_t       ClientID;

    /**
      * Size of a JobUnit, meaning is user defined. Generally will indicate
      * amount of bytes.
      */
    typedef size_t       JobUnitSize;

    enum ClientState
    {
        kBusy,
        kIdle
    };

    /**
      * Size of header for a job unit packet. For now it's just twice the length
      * of a single number, since the header consists of id + size.
      */
    const JobUnitSize HEADER_LENGTH = 2 * sizeof(size_t);


    typedef size_t ResponseCode;

    /**
      * Possible response codes.
      */
    const ResponseCode JobUnitCompleted  = 13;
    const ResponseCode ConnectionSuccess = 14;

    /**
      * Size of header for a response packet. Just twice the length
      * of a single number, since the header consists of a ResponseCode.
      */
    const JobUnitSize RESPONSE_HEADER_LENGTH = sizeof(size_t);

    /**
      * Maximum size of a response.
      */
    const JobUnitSize MAX_RESPONSE_LENGTH = 50;

    /**
      * Maximum size of the Job Unit Queue.
      */
    const size_t MAX_JOBUNITS_QUEUE_SIZE   = 11;

    /** Used to identify a method. */
    typedef std::string  MethodDescriptor;

    /** A Distance is a simple scalar type. */
    typedef float    Distance;

    /** 
      * Root Mean Square Deviation. Represents a measure of distance 
      * between the backbones of two superimposed proteins.
      */
    typedef Distance RMSD;

    /** A Mass is a simple scalar type. */
    typedef float    Mass;

    /** A Mass vector. Non-scalar type. */
    typedef std::vector<Mass> Masses;

    /** A Coord is a scalar type. */
    typedef float Coord;

    /** An angle is a scalar type. */
    typedef float Angle;

    /** Max size of a single Job Unit. */
    static const size_t JOB_UNIT_SIZE    = 50;

    /** Message constants. Deprecated, not in use.*/
    namespace Messages {
        static const StringMessage END(std::string("\0\0\0\0\0\0\0\0",8));
        static const StringMessage OK("OK ");
        static const StringMessage BAD("BAD ");
    }

    /** Port options. */
    typedef size_t Port;
}

#endif