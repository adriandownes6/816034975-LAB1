/* Adrian Downes/816034975 */
#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/timer.h"
#include <ctype.h>
#include <unistd.h>
#include <sys/times.h> // gcc cyclicx.c -o cyclicx
#define SLOTX 4
#define CYCLEX 5
#define SLOT_T 5000 // 5 sec slot time
int tps, cycle=0, slot=0;
clock_t now, then;
struct tms n;
//Sleep function
void sleep(int ms)
{
    sleep_ms(ms); //Pico function to sleep in milliseconds
}
void one() { // task code
printf("task 1 running\n");
sleep(1000);
}
void two() {
printf("task 2 running\n");
sleep(2000);
}
void three() {
printf("task 3 running\n");
sleep(3000);
}
void four() {
printf("task 4 running\n");
sleep(4000);
}
void five() 
{
printf("task 5 running\n");
sleep(5000);
}
void burn() 
{
    clock_t bstart = times(&n);
    while ( ((now=times(&n))-then) < SLOT_T*tps/1000 ) 
    {
        /* burn time here */
    }
    printf("burn time = %2.2dms\n\n", (times(&n)-bstart)*1000/tps);
    then = now;
    cycle = CYCLEX;
}
void (*ttable[SLOTX][CYCLEX])() = {
    {one, two, burn, burn, burn},
    {one, three, burn, burn, burn},
    {one, four, burn, burn, burn},
    {burn, burn, burn, burn, burn}
};
main () 
{
    stdio_init_all(); //Initialize UART for Serial output
    tps = sysconf(_SC_CLK_TCK);
    printf("clock ticks/sec = %d\n\n", tps);
    while (1) 
    {
        for(slot=0; slot<SLOTX; slot++)
        for(cycle=0; cycle<CYCLEX; cycle++)
        (*ttable[slot][cycle])(); // dispatch next task
                                // from table
    }
}