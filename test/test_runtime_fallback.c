#include <p101_util/endian.h>
#include <stddef.h>
#include <stdint.h>

int main(void)
{
    const uint16_t value = UINT16_C(0x0102);
    const uint8_t *bytes;
    int            expected;

    bytes    = (const uint8_t *)&value;
    expected = bytes[0] == UINT8_C(0x02);
    return (p101_is_little_endian(NULL) == expected) ? 0 : 1;
}
