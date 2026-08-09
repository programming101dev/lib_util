#include <p101_util/endian.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

int main(void)
{
    const uint16_t value = UINT16_C(0x0102);
    const uint8_t *bytes;
    bool           expected;
    bool           actual;
    int            status;

    bytes    = (const uint8_t *)&value;
    expected = false;
    if(bytes[0] == UINT8_C(0x02))
    {
        expected = true;
    }
    actual = p101_is_little_endian(NULL);
    status = 1;
    if(actual == expected)
    {
        status = 0;
    }

    return status;
}
