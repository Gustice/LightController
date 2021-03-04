#include <stdarg.h>
#include <stdio.h>
#include <string.h>

#include "esp_log.h"
#include "esp_err.h"

int printfLine(char const *format, ...) { // This takes VarArgs
    int retVal;
    va_list args;
    va_start(args, format);
    retVal = vprintf(format, args);
    printf("\n");
    va_end(args);
    return (retVal);
}
