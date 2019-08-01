#include "balance.h"
#include <stdint.h>


extern "C" int LLVMFuzzerTestOneInput(const uint8_t *Data, size_t Size) {
    int error;
    char result[1024];

    if(Size < (1 + sizeof(int)))
            return 0;

    if (Data[Size - 1] != '\0')
        return 0;

    error = team_balance(
        reinterpret_cast<const char*>(Data + sizeof(int)),
        *reinterpret_cast<const int*>(Data),
        result, sizeof(result)
    );
    return 0;
}
