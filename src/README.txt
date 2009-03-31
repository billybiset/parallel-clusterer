For now, just the structure, coord3d and rmsd files plus the Doxygen config file.

So, this should compile: g++ -c structure.cpp

Notes: Only structure.h is properly documented for now, some lines were commented at
       the end of structure.cpp for ISO C++ compliance. The coor3d and rmsd files only
       include a file header but no additional comments/documentation.

Also, you can generate the documentation with : doxygen Doxyfile

Documentation output will be to <project-main>/doc/html (see Doxyfile for config options).