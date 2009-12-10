/**
 * \file  xtc2pdb.cpp
 * \brief Tool for converting .xtc files to .pdb format.
 *
 * FDC: FuDePAN Distributed Clusterer
 * <http://fud.googlecode.com/>
 * Copyright (C) 2009 Guillermo Biset, FuDePAN
 *
 * This file is part of the FuD project.
 *
 * Contents:       Tool for converting .xtc file to gnuplot command file.
 *
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

#include <iostream>
#include <iomanip>
#include <fstream>
#include <cstring>

extern "C"
{
    #include "xdrfile.h"
    #include "xdrfile_xtc.h"
}

#include "getopt_pp.h"

using namespace GetOpt;

void show_help()
{
    std::cout << "xtc2pbd generates a .pdb file from an .xtc one.\n"
        "\t Example: xtc2pdb -i input.xtc -o output.pdb\n\n"
        "Valid options are:\n"
        "\t-i --input                  Input .xtc file.\n"
        "\t-o --output                 Output .pdb file.\n"
        "\t-h --help                   Show this help and exits.\n\n"
    ;
}

void output(XDRFILE* xdrfile, int atoms, std::string input, std::string output_file)
{
    std::ofstream file;
    file.open( output_file.c_str() );

    std::vector<rvec*> proteins;

    bool _finished_reading(false);

    int result;

    while (!_finished_reading)
    {
        rvec   *_atoms_vector =  new rvec[atoms];
        int    step;
        float  time;
        matrix box;
        float  prec;


        result = read_xtc(xdrfile,atoms,&step,&time,box,_atoms_vector,&prec);

        if (exdrENDOFFILE == result)
            _finished_reading = true;
        else if (exdrOK != result)
        {
            std::cerr << "ERROR: read_xtc " << result << std::endl;
            _finished_reading = true;
        }
        else
            proteins.push_back(_atoms_vector);
    }

    //output to file
    size_t protein;
    int triple;

    size_t t(0);

    for(protein = 0; protein < proteins.size() - 1; ++protein)
    {
        file << "REMARK    Generated by xtc2pdb\n"
             << "TITLE     t=0.000 t= " <<  t << ".00000\n"
             << "MODEL        " << protein << std::endl;

        t += 500;

        int atom;
        atom = 1;

        triple = 1;

        while (atom <= atoms)
        {
            file << "ATOM " << std::setw(5) << atom << " N   ALA " << std::setw(5) << triple
                 << std::setw(8) << proteins[protein][atom-1][0] << ' '
                 << std::setw(8) << proteins[protein][atom-1][1] << ' '
                 << std::setw(8) << proteins[protein][atom-1][2] << std::endl;

            ++atom;

            file << "ATOM " << std::setw(5) << atom << " CA  ALA " << std::setw(5) << triple
                 << std::setw(8) << proteins[protein][atom-1][0] << ' '
                 << std::setw(8) << proteins[protein][atom-1][1] << ' '
                 << std::setw(8) << proteins[protein][atom-1][2] << std::endl;

            ++atom;

            file << "ATOM " << std::setw(5) << atom << " C   ALA " << std::setw(5) << triple
                 << std::setw(8) << proteins[protein][atom-1][0] << ' '
                 << std::setw(8) << proteins[protein][atom-1][1] << ' '
                 << std::setw(8) << proteins[protein][atom-1][2] << std::endl;

            ++atom;
            ++triple;
        }
        file << "TER\nENDMDL\n";
    }
    file.close();
}


int main(int argc, char *argv[])
{
    GetOpt_pp options(argc,argv);

    if (! (options >> OptionPresent('i',"input")) || ! (options >> OptionPresent('o',"output")) )
        show_help();
    else
    {
        std::string output_file;
        std::string input;

        size_t first(0);
        size_t total(0);

        options >> Option('i',"input",input) >> Option('o',"output",output_file)
                >> Option('f',"first",first) >> Option('t',"total",total);

        int atoms;
        int result;

        // You can't always get what you want...
        std::string str(input);
        char *fname = new char[str.size() + 1];
        strcpy(fname,str.c_str());
        // ...(RS)

        result = read_xtc_natoms(fname,&atoms);

        if (exdrOK != result)
            std::cerr <<"Error initializing database. Wrong filename?\n";

        XDRFILE* xdrfile;

        xdrfile = xdrfile_open(input.c_str(),"r");
        if (NULL == xdrfile)
            std::cerr << "Error opening file.\n";
        else
            output(xdrfile,atoms,input,output_file);
    }
}