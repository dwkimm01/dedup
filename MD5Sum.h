#pragma once

#include <string>
#include "md5.h"

namespace common 
{

class MD5Sum
{
public:
    MD5Sum();
    
    ~MD5Sum();

    void update(const char* buffer, const int bufferLength);

    class Digest
    {
    public:
        unsigned char data[16];
    };
    Digest getDigest() const;
    std::string getHexStringDigest() const
    {
        Digest dig = getDigest();
        return toHexString(dig.data, sizeof(dig.data));
    }

    static std::string toHexString(const unsigned char* buffer, const unsigned int bufferLength);

    void clear();

private:

    MD5 md5_;
};

} // namespace common 

