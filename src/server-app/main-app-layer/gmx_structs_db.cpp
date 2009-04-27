#include <iostream>
#include <string.h>
#include <stdexcept>
#include "structure.h"
#include "gmx_structs_db.h"


extern "C" {
#include <xtcio.h>
#include <types/simple.h>
}
#undef bool

GmxStructsDB::GmxHandler::GmxHandler(const string& filename, AccessMode mode, size_t* natoms_to_fill)
	: first_read(true), access_mode(mode), _natoms_to_fill(natoms_to_fill)
{
	char* const dup = strdup(filename.c_str());	// GMX non consts usage :(
    char* smode[3] = {"r", "w", "rw" };
//     string smode[3] = {"r", "w", "rw" };

//     char mode_c_str[3];                        //I made some changes here, to remove g++ warnings 
                                               //about c char* vs. c++ strings
//     strcpy(mode_c_str, smode[mode]); //.c_str()); //be careful with strcpy!
	
	handle = open_xtc(dup, smode[mode]);
	free(dup);
//	if (handle == 0) throw std::runtime_error("GMX_DB: file not found");
}

GmxStructsDB::GmxHandler::~GmxHandler()
{
	close_xtc(handle);
}

extern "C" {
void rewind_trj(int handle);
}

void GmxStructsDB::GmxHandler::move_first()
{
    if (!first_read)
    {
		rewind_trj(handle);
    }
    //else
	//first_read = true;
	SequentialDB_handler::move_first();
}

bool GmxStructsDB::GmxHandler::read_next(EditableCoordsStructure& struc)
{
	bool   ret(true);
	matrix box;
	int    /*nframe, */step;
	real   time = 0.0;
	int    bOK = 1;
	
	
	if (access_mode != kReadOnly && access_mode != kReadWrite)
	    throw std::runtime_error("GmxStructsDB::read_next in invalid access mode");
	
	SequentialDB_handler::read_next(struc);
	
	struc.clear();
	
	if (first_read)
	{
		int natoms = 0;
		first_read = false;
		read_first_xtc(handle,&natoms,&step,&time,box,&vec,&last_prec,&bOK);
		atoms_count = natoms;
		if (_natoms_to_fill != NULL)
			*_natoms_to_fill = atoms_count;
	}
	else
	{
		ret = read_next_xtc(handle,atoms_count,&step,&time,box,vec,&last_prec,&bOK) != 0;
	}
	
	if (bOK != 0) /* OK */
	{
		if (ret)
			fill_structure(struc);
		return ret;
	}
	else
		throw std::runtime_error("GMX_DB: Error reading next protein");
}


void GmxStructsDB::GmxHandler::fill_structure(EditableCoordsStructure& s) const
{
	s.set_size(atoms_count);
	for(unsigned int atom = 0; atom < atoms_count; atom++)
	{
		s.set_coord(
			    atom, 
			    Coord3D(vec[atom][XX], vec[atom][YY], vec[atom][ZZ])
		);
	}
}

void GmxStructsDB::GmxHandler::write_next(const Structure& struc)
{
    if (access_mode != kWriteOnly && access_mode != kReadWrite)
        throw std::runtime_error("GmxStructsDB::write_next in invalid access mode");

    const size_t natoms = struc.get_natoms();
    rvec* atoms = new rvec[natoms];
    /*matrix box;*/
    
    SequentialDB_handler::write_next(struc);
    
    // convert from struc to rvec[]
    for (size_t i=0; i<natoms; ++i)
    {
	const Coord3D& coord = struc.get_coord(i);
	
	atoms[i][XX] = coord.x / 10.0;
	atoms[i][YY] = coord.y / 10.0;
	atoms[i][ZZ] = coord.z / 10.0;
    }
    
    const int res = writextc(atoms, natoms);
    
    delete [] atoms;
    
    if (res != 0)	// OK
    {
	last_step++;
    }
    else
	throw std::runtime_error("GMX_DB: Error writing next protein");

}

void GmxStructsDB::GmxHandler::write_nth(size_t index, const Structure& struc)
{
	throw std::runtime_error("GMX_DB: write_nth not implemented yet");
}

int GmxStructsDB::GmxHandler::writextc(rvec* atoms, size_t natoms)
{
    rvec box[3];

    box[0][XX]= 9.0;  box[0][YY]= 0.0;  box[0][ZZ]= 0.0;
    box[1][XX]= 0.0;  box[1][YY]= 9.0;  box[1][ZZ]= 0.0;
    box[2][XX]= 0.0;  box[2][YY]= 0.0;  box[2][ZZ]= 9.0;
    
    return write_xtc(handle, natoms, last_step, (float) last_step*1.0, box, atoms, 1000.0);
}

/*
StructuresDB* GmxStructsDB::clone() const
{
    return new GmxStructsDB(last_prec);
}

*/
