#include <stdio.h>

int main()
{
    int n = 1; // stored as 000.0001(big) or 100.000(little)
    if (*(char *)&n == 1) // (char *)&num points to first byte of n
    {
        printf("little"); // if first byte reads 1, little endian
    }
    else
    {
        printf("big"); // else big endian
    }
}