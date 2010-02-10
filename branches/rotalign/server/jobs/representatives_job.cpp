#include <fstream>
#include <iostream>

#include <syslog.h>

extern "C"
{
    #include "xdrfile.h"
}

#include "mili/mili.h"
#include "representatives_job.h"
#include "protein.h"

using namespace fud;
using namespace clusterer;

RepresentativesJob::RepresentativesJob(ProteinDatabase& protein_db, float cutoff) :
    DistributableJobImplementation(),
    _protein_db(protein_db),
    _atoms_in_a_protein(protein_db.get_atom_number()),
    _marked_ids(),
    _next_protein(0),
    _finished(false),
    _cutoff(cutoff)
{
}


const char* RepresentativesJob::get_name() const
{
    return "RepresentativesJob";
}

void RepresentativesJob::handle_results (JobUnitID id,InputMessage& input)
{
    std::vector<ProteinID> new_marked;

    input >> new_marked;

    for (std::vector<ProteinID>::iterator it(new_marked.begin()); it != new_marked.end(); ++it)
    {
        bool inside(false);
        std::vector<ProteinID>::iterator it2(_marked_ids.begin());
        while (! inside && it2 != _marked_ids.end())
            inside = _protein_db[*it].rmsd_to(_protein_db[*(it2++) ]) <= _cutoff;

        if (! inside)
            _marked_ids.push_back(*it);
    }
}

void RepresentativesJob::output_results() const
{
    std::cout << "Cluster total: " << _marked_ids.size() << std::endl << "[";
    for (size_t i(0); i < _marked_ids.size(); ++i)
        std::cout << _marked_ids[i] << ( (i == _marked_ids.size() - 1) ? ']':',');

    std::cout << std::endl;
}

DistributableJobStatus RepresentativesJob::get_status() const
{
    if (_finished)
        return FinishedGenerating;
    else
        return ReadyToProduce;
}

JobUnit* RepresentativesJob::produce_next_job_unit(JobUnitSize size)
{
    std::pair<size_t,size_t> range_pos(_protein_db.generate_elements(_next_protein,size));

    if (range_pos.first != range_pos.second)
    {
        JobUnit* res = new RepresentativesJobJobUnit(_protein_db, _protein_db.get_iterator_pair(range_pos.first,range_pos.second) ,_cutoff,_marked_ids);

        _next_protein = range_pos.second;

        _finished    = _protein_db.finished_reading() && _next_protein >= _protein_db.size();

        return res;
    }
    else
    {
        _finished = true;
        return NULL;
    }
}


/*------- JobUnit methods -------*/

RepresentativesJob::RepresentativesJobJobUnit::RepresentativesJobJobUnit(ProteinDatabase& db, IteratorRange range, float cutoff, const std::vector<ProteinID>& marked_ids) :
    JobUnit(),
    _bos()
{
    _bos << Representatives;

    // Add all the proteins with marked ids
    _bos << cutoff << marked_ids.size();
    for (size_t i(0); i < marked_ids.size(); ++i)
        _bos << db[ marked_ids[i] ];

    // Add proteins that are not marked or checked
    _bos << std::distance(range.first,range.second);
    for (ProteinIterator it(range.first); it != range.second; ++it)
        _bos << *it;

    set_size(marked_ids.size() + std::distance(range.first,range.second));

//     std:: cerr << "Inserted " << db->get_marked_ids().size() << " marked, and " << vector_slice.second - vector_slice.first << " unmared." << std::endl;
}

const std::string& RepresentativesJob::RepresentativesJobJobUnit::get_message() const
{
    return _bos.str();
}

const std::vector<ProteinID>&  RepresentativesJob::get_marked_ids_vector() const
{
    return _marked_ids;
}