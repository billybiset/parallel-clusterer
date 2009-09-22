#ifndef EVENTS_H
#define EVENTS_H

#include "common.h"


namespace parallel_clusterer
{
    class DistributableJob;

    template <class Interface>
    struct DeferredEvent
    {
        virtual void call(Interface* i) = 0;
    };

    template <class Interface>
    class DeferredEvent0Param : public DeferredEvent<Interface>
    {
        void (Interface::*method)();

        virtual void call(Interface* i) { (i->*method)(); }
    public:
        DeferredEvent0Param(void (Interface::*m)()) : method(m){}
    };


    template <class Interface, class Param>
    class DeferredEvent1Param : public DeferredEvent<Interface>
    {
        void (Interface::*method)(Param p);
        Param const p;

        virtual void call(Interface* i) { (i->*method)(p); }
    public:
        DeferredEvent1Param(void (Interface::*m)(Param), Param p) : method(m), p(p){}
    };

    template <class Interface, class Param1, class Param2>
    class DeferredEvent2Param : public DeferredEvent<Interface>
    {
        void (Interface::*method)(Param1 p1,Param2 p2);
        Param1 const p1;
        Param2 const p2;

        virtual void call(Interface* i) { (i->*method)(p1,p2); }
    public:
        DeferredEvent2Param(void (Interface::*m)(Param1,Param2), Param1 p1, Param2 p2) : method(m), p1(p1), p2(p2){}
    };

}

#endif