#include <queue>
#include <boost/thread/mutex.hpp>

#ifndef SYNCHRONIZED_QUEUE_H
#define SYNCHRONIZED_QUEUE_H

    template <class T>
    class SynchronizedQueue : private std::queue<T>
    {
        public:
            SynchronizedQueue() :
                std::queue<T>(),
                _mutex()
            {
            }

            SynchronizedQueue(const SynchronizedQueue& other)
            {
            }

            T& front()
            {
                boost::mutex::scoped_lock lock(_mutex);
                return std::queue<T>::front();
            }

            T& back()
            {
                boost::mutex::scoped_lock lock(_mutex);
                return std::queue<T>::back();
            }

            size_t size()
            {
                boost::mutex::scoped_lock lock(_mutex);
                return std::queue<T>::size();
            }

            void push(const T& x)
            {
                boost::mutex::scoped_lock lock(_mutex);
                std::queue<T>::push(x);
            }

            void pop()
            {
                boost::mutex::scoped_lock lock(_mutex);
                std::queue<T>::pop();
            }

            bool empty()
            {
                boost::mutex::scoped_lock lock(_mutex);
                return std::queue<T>::empty();
            }

        private:
            boost::mutex _mutex;
    };

#endif