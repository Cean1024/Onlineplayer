#ifndef MD5SUM_H
#define MD5SUM_H

#include <string>
#include <fstream>
#include<cstring>
/* Type define */
typedef unsigned char byte;
typedef unsigned int uint32;

using std::string;
using std::ifstream;

class Md5sum
{
public:
    Md5sum();
    Md5sum(const void *input, size_t length);
    Md5sum(const string &str);
    Md5sum(ifstream &in);
    void update(const void *input, size_t length);
    void update(const string &str);
    void update(ifstream &in);
    const byte* digest();
    string toString();
    void reset();
protected:
    void update(const byte *input, size_t length);
    void final();
    void transform(const byte block[64]);
    void encode(const uint32 *input, byte *output, size_t length);
    void decode(const byte *input, uint32 *output, size_t length);
    string bytesToHexString(const byte *input, size_t length);

    /* class uncopyable */
    Md5sum(const Md5sum&);
    Md5sum& operator=(const Md5sum&);
private:
    uint32 _state[4];   /* state (ABCD) */
    uint32 _count[2];   /* number of bits, modulo 2^64 (low-order word first) */
    byte _buffer[64];   /* input buffer */
    byte _digest[16];   /* message digest */
    bool _finished;     /* calculate finished ? */

    static const byte PADDING[64];  /* padding for calculate */
    static const char HEX[16];
    static const size_t BUFFER_SIZE = 1024;
};

#endif // MD5SUM_H
