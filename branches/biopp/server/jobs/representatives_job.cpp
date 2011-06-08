#include <fstream>

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
    if ( _finished )
        return NULL;
    else
    {
        StreamingJobUnit* res = new StreamingJobUnit();

        (*res) << Representatives;

        // Add all the proteins with marked ids
        (*res) << _cutoff << fud_uint(_marked_ids.size());
        for (size_t i(0); i < _marked_ids.size(); ++i)
            (*res) << _protein_db[ _marked_ids[i] ];

        const ProteinID begin = _next_protein;
        ProteinID end;

        size_t c = 0;
        Protein* p = NULL;

        while ((c < size) && ((p = _protein_db.read(begin + c)) != NULL))
        {
            ++c;
        }
        end = begin + c;

        if (p == NULL)
            --end;

        //how many there will be?
        (*res) <<  fud_uint(end - begin);

        for (size_t i(begin); i < end; ++i)
            (*res) << _protein_db[ i ];

        res->set_size(_marked_ids.size() + (end - begin));

        _next_protein = end;
        _finished     = p == NULL && _next_protein >= _protein_db.size() - 1;

        return res;
    }
}

const std::vector<ProteinID>&  RepresentativesJob::get_marked_ids_vector() const
{
    return _marked_ids;
}
