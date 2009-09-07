#include <boost/thread.hpp>
#include "synchronized_containers.h"

template <class EventType>
class Consumer
{
private:
    LockingQueue<EventType*> incoming_events;

protected:

    Consumer() :
        incoming_events()
    {
    }

    EventType* wait_for_event()
    {
        return incoming_events.wait_for_element();
    }
public:
    void push_event(EventType* event)
    {
        incoming_events.push(event);
    }
};

template <class EventType>
class Producer
{
    Consumer<EventType>* const cons;
protected:
    Producer(Consumer<EventType>* consumer) :
        cons(consumer)
    {
    }

    void send_event(EventType* event)
    {
        cons->push_event(event);
    }
};


struct SchedulerInterface
{
    virtual void event1(int)        = 0;
    virtual void event2(int, char)  = 0;
};


struct SchedulerCaller
{
   virtual void call(SchedulerInterface* scheduler) = 0;
};


typedef SchedulerCaller SchedulerEvent;

class SchedulerSender :
    public SchedulerInterface,
    public Producer<SchedulerEvent>
{
    virtual void event1(int argument)
    {
        struct Event :
            public SchedulerCaller
        {
            const int argument;

            Event(int arg) :
                argument(arg)
            {
            }

            virtual void call(SchedulerInterface* scheduler)
            {
                scheduler->event1(argument);
            }
        };

        send_event(new Event(argument));
    }

    virtual void event2(int arg1, char arg2)
    {
        struct Event :
            public SchedulerCaller
        {
            const int  arg1;
            const char arg2;

            Event(int arg1, char arg2) :
                arg1(arg1),
                arg2(arg2)
            {
            }

            virtual void call(SchedulerInterface* scheduler)
            {
                scheduler->event2(arg1, arg2);
            }
        };

        send_event(new Event(arg1, arg2));
    }
public:
    SchedulerSender(Consumer<SchedulerEvent>& cons) :
        Producer<SchedulerEvent>(&cons)
    {
    }
};

class Scheduler :
           private SchedulerInterface,
           public Consumer<SchedulerEvent>
           /* ojo, de haber multiple consumicion, deja de ser herencia, y el metodo deja de ser virtual */
{
private:
    virtual void event1(int arg)
    {
        cout << "Event1, arg: " << arg << endl;
    }

    virtual void event2(int arg1, char arg2)
    {
        cout << "Event2, arg1: " << arg1 << " arg2: " << arg2 << endl;
    }

public:
   void work()
   {
       SchedulerEvent* event;

       while(true)
       {
            event = wait_for_event();
            if (event != NULL)
            {
                event->call(this);
                delete event;
            }
       }

   }
};


class L1Algo
{
   SchedulerInterface* const scheduler_interface;

public:
    L1Algo (SchedulerInterface* interface) :
        scheduler_interface(interface)
    {
    }

    void generate_ev1(unsigned int amount, unsigned int seconds)
    {
        while (amount-- > 0)
        {
            sleep(seconds);
            scheduler_interface->event1( 90 - amount);
        }
    }

    void generate_ev2(unsigned int amount, unsigned int seconds)
    {
        while (amount-- > 0)
        {
            sleep(seconds);
            scheduler_interface->event2(123+amount, 70 - amount);
        }
    }

};
