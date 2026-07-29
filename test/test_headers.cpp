#include <cstdint>
#include <p101_util/endian.h>

int main()
{
    std::uint16_t (*typed_bswap16)(const p101_env *, std::uint16_t);
    int (*typed_is_little_endian)(const p101_env *);

    typed_bswap16          = p101_bswap16;
    typed_is_little_endian = p101_is_little_endian;
    (void)typed_bswap16;
    (void)typed_is_little_endian;
    return 0;
}
