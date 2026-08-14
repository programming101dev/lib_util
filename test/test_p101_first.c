#include <p101_endian/endian.h>
#include <stddef.h>

#if defined(__APPLE__) || defined(__FreeBSD__)
    #include <sys/endian.h>
#elif defined(__linux__)
    #include <byteswap.h>
    #include <endian.h>
#endif

int main(void)
{
    return (p101_bswap32(NULL, UINT32_C(0x12345678)) == UINT32_C(0x78563412)) ? 0 : 1;
}
