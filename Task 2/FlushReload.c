#include <emmintrin.h>
#include <x86intrin.h>
#include <stdint.h>
#include <stdio.h>
#include <stdbool.h>


uint8_t array[256*4096];
int temp;
char secret = 94;

/* cache hit time threshold assumed*/
#define CACHE_HIT_THRESHOLD (250)
#define DELTA 1024

void flushSideChannel() {
    int i;
    
    // Write to array to bring it to RAM to prevent Copy-on-write
    for (i = 0; i < 256; i++) array[i*4096 + DELTA] = 1;

    for (i = 0; i < 256; i++) _mm_clflush(&array[i*4096 +DELTA]);
}

void victim() {
    temp = array[secret*4096 + DELTA];
}

bool reloadSideChannel() {
    int junk=0;
    bool attack_res = false;
    register uint64_t time1, time2;
    volatile uint8_t *addr;
    int i;
    for(i = 0; i < 256; i++){
        addr = &array[i*4096 + DELTA];
        time1 = __rdtscp(&junk);
        junk = *addr;
        time2 = __rdtscp(&junk) - time1;
        if (time2 <= CACHE_HIT_THRESHOLD){
            attack_res = true;
            //printf("array[%d*4096 + %d] is in cache.\n", i, DELTA);
            //printf("The Secret = %d.\n",i);
        }
    }
    
    return attack_res;
}

int main(int argc, const char **argv) {
    int nsuc = 0;

    for(int i = 0; i < 100; i++){
        flushSideChannel();
        victim();

        nsuc += reloadSideChannel() ? 1 : 0;
    }

    printf("Attack Success Rate 100 Rounds: %.2f\n", (float)nsuc/100.0);
    return (0);
}