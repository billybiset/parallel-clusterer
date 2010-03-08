/**
 * \file  xtc_reader.h
 * \brief Definition of XtcReader class.
 *
 * FDC: FuDePAN Distributed Clusterer
 * <http://fud.googlecode.com/>
 * Copyright (C) 2010 Guillermo Biset, FuDePAN
 *
 * This file is part of the FDC project.
 *
 * Contents:       Header file for FDC providing class XtcReader.
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

#include "structure_reader.h"

extern "C"
{
    #include "xdrfile.h"
}

#ifndef XTC_READER_H
#define XTC_READER_H


namespace clusterer
{
    class XtcReader : public StructureReader
    {
        public:
            XtcReader(const char* file_name) throw (const char*);

        private:
            //Inherited methods from StructureReader:
            virtual void   read_structure(Protein& protein);

            virtual std::vector<float> get_box() const;

            virtual size_t get_atom_number()  const;
            virtual float  get_precision()    const;
            virtual bool   finished_reading() const;

            //Attributes:
            bool                     _finished_reading;
            XDRFILE*                 _xd;
            int                      _atoms_in_a_protein;
            std::vector<float>       _box;
            float                    _precision;
    };
}

#endif
