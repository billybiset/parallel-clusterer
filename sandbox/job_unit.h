#ifndef JOB_UNIT_H
#define JOB_UNIT_H

typedef unsigned long JobUnitID;

class JobUnit
{
    public:
        virtual       void   print_info()           const = 0;
        virtual const char * method_name_required() const = 0;

        unsigned      long   get_id()               const;
    protected:
        JobUnit();
    private:
        JobUnitID _id;
};

#endif