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

    struct ClientsManagerInterface
    {
        virtual void free_client_event()        = 0;
    };

    template<class Interface>
    struct Caller
    {
        virtual void call(Interface* scheduler) = 0;
        virtual ~Caller(){}
    };

    class ClientsManagerSender :
        public ClientsManagerInterface,
        public Producer<Caller<ClientsManagerInterface> >
    {
        public:
            virtual void free_client_event()
            {
                struct Event : public Caller<ClientsManagerInterface>
                {
                    Event() {}

                    virtual void call(ClientsManagerInterface* scheduler)
                    {
                        scheduler->free_client_event();
                    }
                };
                send_event(new Event());
            }

            ClientsManagerSender(Consumer<Caller<ClientsManagerInterface> >& cons) :
                Producer<Caller<ClientsManagerInterface> >(&cons)
            {
            }
    };

#endif