#include <emmintrin.h>
#include <x86intrin.h>
#include <stdint.h>
#include <stdio.h>

uint8_t array[10*4096];

void process(int* data){
    int junk=0;
    register uint64_t time1, time2;
    volatile uint8_t *addr;
    int i;
    // Initialize the array
    for(i=0; i<10; i++) array[i*4096]=1;

    // FLUSH the array from the CPU cache
    for(i=0; i<10; i++) _mm_clflush(&array[i*4096]);

    // Access some of the array items
    array[3*4096] = 100;
    array[7*4096] = 200;

    for(i=0; i<10; i++) {
        addr = &array[i*4096];
        time1 = __rdtscp(&junk); 
        junk = *addr;
        time2 = __rdtscp(&junk) - time1;
        data[i] = (int)time2;
    }

}

int main(int argc, const char **argv) {
    int* cumulative = malloc(10 * sizeof(int));

    for(int i = 0; i < 100; i++) {
        int* run = malloc(10 * sizeof(int));
        process(run);
        for(int j = 0; j < 10; j++) cumulative[j] += run[j];

        free(run);
    }

    for(int i = 0; i < 10; i++) printf("Access time for array[%d*4096]: %d CPU cycles\n",i, (int)(cumulative[i]/100));
}