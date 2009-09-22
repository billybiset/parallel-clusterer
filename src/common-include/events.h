#ifndef EVENTS_H
#define EVENTS_H

#include "common.h"


namespace parallel_clusterer
{
    class DistributableJob;

    template <class Interface>
    struct Event
    {
        virtual void call(Interface* i) = 0;
    };

    template <class Interface>
    class Event0Param : public Event<Interface>
    {
        void (Interface::*method)();

        virtual void call(Interface* i) { (i->*method)(); }
    public:
        Event0Param(void (Interface::*m)()) : method(m){}
    };


    template <class Interface, class Param>
    class Event1Param : public Event<Interface>
    {
        void (Interface::*method)(Param p);
        Param const p;

        virtual void call(Interface* i) { (i->*method)(p); }
    public:
        Event1Param(void (Interface::*m)(Param), Param p) : method(m), p(p){}
    };

    template <class Interface, class Param1, class Param2>
    class Event2Param : public Event<Interface>
    {
        void (Interface::*method)(Param1 p1,Param2 p2);
        Param1 const p1;
        Param2 const p2;

        virtual void call(Interface* i) { (i->*method)(p1,p2); }
    public:
        Event2Param(void (Interface::*m)(Param1,Param2), Param1 p1, Param2 p2) : method(m), p1(p1), p2(p2){}
    };

    template <class Interface>
    inline Event<Interface>* new_event(void (Interface::*method)())
    {
        return new Event0Param<Interface>(method);
    }


    template <class Interface, class Param>
    inline Event<Interface>* new_event(void (Interface::*method)(Param), Param p)
    {
        return new Event1Param<Interface,Param>(method, p);
    }

    template <class Interface, class Param1, class Param2>
    inline Event<Interface>* new_event(void (Interface::*method)(Param1,Param2), Param1 p1,Param2 p2)
    {
        return new Event2Param<Interface,Param1,Param2>(method, p1,p2);
    }

}

#endif