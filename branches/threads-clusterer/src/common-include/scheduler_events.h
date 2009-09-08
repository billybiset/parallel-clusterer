#ifndef SCHEDULER_EVENTS_H
#define SCHEDULER_EVENTS_H

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
        virtual void free_client_event()        = 0;
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
        virtual void free_client_event()
        {
            struct Event :
                public SchedulerCaller
            {
                Event() {}

                virtual void call(SchedulerInterface* scheduler)
                {
                    scheduler->free_client_event();
                }
            };

            send_event(new Event());
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
        virtual void free_client_event()
        {
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

    };

#endif