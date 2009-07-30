#include "binstream.h"

//C++, I hate you
template BOStream& BOStream::operator << <int>(int x);
template BOStream& BOStream::operator << <unsigned int>(unsigned int x);

template <class T>
BOStream& BOStream::operator<< (T x)
{
    _s.append(reinterpret_cast<char*>(&x), sizeof(T));
    return *this;
}

BOStream& BOStream::operator<< (const std::string& s)
{
    (*this) << s.size();
    _s += s;
    return *this;
}

BOStream& BOStream::operator<< (const char* cs)
{
    const std::string s(cs);
    return operator<< (s);
}

std::string BOStream::str() const
{
    return _s;
}

BIStream::BIStream(std::string str) :
    _s(str),
    _pos(0)
{
}

//I still hate you
template BIStream& BIStream::operator >> <int>(int& x);
template BIStream& BIStream::operator >> <unsigned int>(unsigned int& x);

template <class T>
BIStream& BIStream::operator >> (T& x)
{
    _pos += _s.copy(reinterpret_cast<char*>(&x), sizeof(x),_pos);
    return *this;
}

BIStream& BIStream::operator >> (std::string& str)
{
    int size;
    (*this) >> size;
    str  = _s.substr(_pos,size);
    _pos += size;
    return *this;
}

