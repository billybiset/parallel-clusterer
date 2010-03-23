/**
 * \file  structure_reader.h
 * \brief Definition of StructureReader class.
 *
 * FDC: FuDePAN Distributed Clusterer
 * <http://fud.googlecode.com/>
 * Copyright (C) 2010 Guillermo Biset, FuDePAN
 *
 * This file is part of the FDC project.
 *
 * Contents:       Header file for FDC providing class StructureReader.
 *
 * System:         FDC
 * Language:       C++
 *
 * Author:         Guillermo Biset
 * E-Mail:         billybiset AT gmail.com
 *
 * FDC is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * FDC is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with FuD.  If not, see <http://www.gnu.org/licenses/>.
 *
 */

#include <vector>

#include "protein.h"

#ifndef STRUCTURE_READER_H
#define STRUCTURE_READER_H

namespace clusterer
{
    struct StructureReader
    {
        virtual void   read_structure(Protein& protein) = 0;

        virtual float* get_box()                        = 0;

        virtual size_t get_atom_number()          const = 0;
        virtual float  get_precision()            const = 0;
        virtual bool   finished_reading()         const = 0;
    };
}

#endif