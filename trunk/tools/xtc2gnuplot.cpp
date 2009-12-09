/**
 * \file  gnuplot_output.cpp
 * \brief Tool for converting .xtc file to gnuplot command file.
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
#include <fstream>
#include <cstring>

extern "C"
{
    #include "xdrfile.h"
    #include "xdrfile_xtc.h"
}

#include "getopt_pp.h"

using namespace GetOpt;

void output(XDRFILE *xdrfile, int atoms, std::string input, std::string output_file, size_t first, size_t total)
{
    int result;

    std::ofstream plotfile;
    plotfile.open( (output_file + ".dat").c_str() );

    std::ofstream commandfile;
    commandfile.open( output_file.c_str() );

    commandfile << "set terminal x11" << std::endl;
    commandfile << "set title \"Showing " << total << " proteins in " << input << ", starting from frame " << first << ".\"" << std::endl;

    for (int o(0);o < atoms; ++o)
    {
        bool _finished_reading(false);

        //discard the initial first proteins
        for (size_t f(0); f < first && !_finished_reading; ++f)
        {
            rvec   _atoms_vector[atoms];
            int    step;
            float  time;
            matrix box;
            float  prec;

            // read one protein
            result = read_xtc(xdrfile,atoms,&step,&time,box,_atoms_vector,&prec);

            if (exdrENDOFFILE == result)
                _finished_reading = true;
            else if (exdrOK != result)
            {
                std::cerr << "ERROR: read_xtc " << result << std::endl;
                _finished_reading = true;
            }
        }

        std::vector<rvec*> proteins;

        for (size_t f(0); f < total && !_finished_reading; ++f)
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

        //output to plot file
        for (size_t atom(0); atom < atoms; ++atom)
        {
            for(size_t protein(0); protein < proteins.size(); ++protein)
                plotfile << proteins[protein][atom][0] << ' ' << proteins[protein][atom][1] << ' ' << proteins[protein][atom][2] << ' ';

            plotfile << std::endl;
        }

        size_t column(1);
        size_t color(1);

        for (size_t protein(0); protein < proteins.size(); ++protein)
        {
            if (protein == 0)
            {
                commandfile << "splot \"" << output_file << ".dat\" using 1:2:3 title \"Frame "<<first + protein <<"\" with lines lw 3 lt 1";

                if (proteins.size() > 1)
                    commandfile << ",\\" << std::endl;
                else
                    commandfile << std::endl;
            }
            else
            {
                if (protein == proteins.size() - 1)
                    commandfile << "\"" << output_file << ".dat\" using " << column <<':' << column+1 << ':' << column+2 << " title \"Frame "<<first + protein<<"\" with lines lw 3 lt " << color  << std::endl;
                else
                    commandfile << "\"" << output_file << ".dat\" using " << column <<':' << column+1 << ':' << column+2 << " title \"Frame "<<first + protein<<"\" with lines lw 3 lt " << color << ",\\" << std::endl;
            }

            column += 3;
            ++color;
        }

        commandfile << "pause -1" << std::endl;

        plotfile.close();
        commandfile.close();
    }
}

void show_help()
{
    std::cout << "xtc2gnuplot generates gnuplot output from .xtc files.\n"
        "\t Example: xtc2gnuplot -i input.xtc -o gnuplot_output\n\n"
        "Valid options are:\n"
        "\t-i --input                  Input xtc file.\n"
        "\t-o --output                 Output prefix. Will generate two files:. If prefix is cluster:\n"
        "\t                            \t - One file will be cluster.dat\n"
        "\t                            \t - The other will be cluster (the original name)\n"
        "\t-f --first       [optional] First frame to output. Default value: 0.\n"
        "\t-t --total       [optional] Amount of frames to output. Default value: all frames.\n"
        "\t-h --help                   Show this help and exits.\n\n"
    ;
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
            output(xdrfile,atoms,input,output_file,first,total);
    }
}