#ifndef PROTEIN_INLINE_H
    #error Internal header file, DO NOT include this.
#endif

//Cast Functions
void Protein::Coord3D2rvec(const Coord3d& coord3d, rvec rv)
{
    rv[0] = coord3d.x;
    rv[1] = coord3d.y;
    rv[2] = coord3d.z;
}


Protein::rvec* Protein::structure2rvec_arr(const std::vector<Coord3d>& s)
{
    const size_t ncoords = s.size();
    rvec* ret = new rvec[ncoords];

    for(size_t i = 0; i < ncoords; ++i)
        Coord3D2rvec(s[i], ret[i]);

    return ret;
}

void Protein::rvec2Coord3D(const rvec rv, Coord3d& coord3d)
{
    coord3d.x = rv[0];
    coord3d.y = rv[1];
    coord3d.z = rv[2];
}

//Simple functions
void Protein::clear_mat(matrix a)
{
    const float nul = 0.0;

    a[0][0] = a[0][1] = a[0][2] = nul;
    a[1][0] = a[1][1] = a[1][2] = nul;
    a[2][0] = a[2][1] = a[2][2] = nul;
}

void Protein::oprod(const rvec a,const rvec b,rvec c)
{
    c[0] = a[1] * b[2] - a[2] * b[1];
    c[1] = a[2] * b[0] - a[0] * b[2];
    c[2] = a[0] * b[1] - a[1] * b[0];
}

void Protein::rotate(double a[][6], size_t i, size_t j, size_t k, size_t l, double tau, double s)
{
    const double g = a[i][j];
    const double h = a[k][l];

    a[i][j] = g - s * (h + g * tau);
    a[k][l] = h + s * (g - h * tau);
}

//Binary stream functions
inline mili::bostream& operator<< (mili::bostream& bos, const ProteinRefWithClusterID& protein)
{
    bos << protein._id << protein._vec;

    return bos;
}


inline mili::bostream& operator<< (mili::bostream& bos, const Protein& protein)
{
    bos << protein._id << protein._atom_vector;

    return bos;
}

inline mili::bistream& operator>> (mili::bistream& bis, Protein& protein)
{
    std::vector<Coord3d> v;
    ProteinID id;
    bis >> id;
    bis >> v;
    protein = Protein(v,id);

    return bis;
}
