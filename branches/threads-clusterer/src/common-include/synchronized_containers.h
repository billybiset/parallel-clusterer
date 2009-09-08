#include <queue>
#include <boost/thread.hpp>
#include <pthread.h>
#include "mili.h"
#include "prepos_mutex.h"

#ifndef SYNCHRONIZED_CONTAINERS_H
#define SYNCHRONIZED_CONTAINERS_H

    template <class T,class Container = std::queue<T> >
    class SynchronizedQueue
    {
    public:
        typedef PrePosCaller<Container* const, PreMutex, PosMutex> SynchQueuePPC;
    private:
        pthread_mutex_t mutex;
        Container       queue;
        PreMutex        premutex;
        PosMutex        posmutex;
        SynchQueuePPC   synchQueuePPC;
    public:
        SynchronizedQueue() :
            queue(),
            premutex(&mutex),
            posmutex(&mutex),
            synchQueuePPC(&queue,premutex,posmutex)
        {
            pthread_mutex_init (&mutex, NULL);
        }

        SynchQueuePPC& operator -> ()
        {
            return synchQueuePPC;
        }

    };

    template <class T,class Container = std::list<T> >
    class SynchronizedList
    {
    public:
        typedef PrePosCaller<Container* const, PreMutex, PosMutex> SynchListPPC;
    private:
        pthread_mutex_t mutex;
        Container       cont;
        PreMutex        premutex;
        PosMutex        posmutex;
        SynchListPPC   synchListPPC;
    public:
        SynchronizedList() :
            cont(),
            premutex(&mutex),
            posmutex(&mutex),
            synchListPPC(&cont,premutex,posmutex)
        {
            pthread_mutex_init (&mutex, NULL);
        }

        SynchListPPC& operator -> ()
        {
            return synchListPPC;
        }
    };


    template <typename T, class Container = std::queue<T> >
    class LockingQueue
    {
        public:
            LockingQueue() :
                _mutex(),
                _condition(),
                _queue()
            {
            }

            LockingQueue(const LockingQueue<T>& other) :
                _mutex(),
                _condition(),
                _queue(other)
            {
            }

            void push(const T& x)
            {
                {
                    boost::lock_guard<boost::mutex> lock(_mutex);
                    _queue->push(x);
                }
                _condition.notify_all();
            }

            T wait_for_element()
            {
                boost::unique_lock<boost::mutex> lock(_mutex);

                while(_queue->empty())
                    _condition.wait(lock);

                T ret = _queue->front();
                _queue->pop();

                return ret;
            }

        private:
            boost::mutex                   _mutex;
            boost::condition_variable      _condition;
            SynchronizedQueue<T,Container> _queue;
    };


#endif