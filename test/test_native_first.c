#if defined(__APPLE__) || defined(__FreeBSD__)
    #include <sys/endian.h>
#elif defined(__linux__)
    #include <byteswap.h>
    #include <endian.h>
#endif

#include <p101_util/endian.h>
#include <stddef.h>

int main(void)
{
    return (p101_bswap16(NULL, UINT16_C(0x1234)) == UINT16_C(0x3412)) ? 0 : 1;
}
