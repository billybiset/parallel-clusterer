#include <string>

#ifndef BINSTREAM_H
#define BINSTREAM_H

class BOStream
{
    public:
        template <class T>
        BOStream& operator << (T x);

        BOStream& operator << (const std::string& s);

        BOStream& operator << (const char* cs);

        std::string str() const;

    private:
        std::string _s;
};

class BIStream
{
    public:
        BIStream(std::string str);

        template <class T>
        BIStream& operator >> (T& x);

        BIStream& operator >> (std::string& str);

    private:
        std::string _s;
        std::size_t _pos;
};

#endif