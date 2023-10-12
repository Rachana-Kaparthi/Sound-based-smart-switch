
#include <time.h>

#define DIFF_MAX 500000

int firstClap = 0;
int secondClap = 0;
int offClap1 = 0;
clock_t firstClaptime;
clock_t secondClaptime;
clock_t offClap1time;
void output(int value);
int sensor_data();

int sensor_data()
{
    int data;
    // Read sensor data into x30
    asm volatile(
            "and %0, x30, 1"
            : "=r"(data)
        );
    return data;
}
void output(int value)
{
    unsigned int mask = 0xFFFFFFFD;
    int value1 = value*2;
    asm volatile(
        "and x30,x30, %1\n\t"
        "or x30,x30, %0\n\t"
        :
        :"r"(value1),"r"(mask)
        :"x30" //cobbler list,indicating that x30 is modified
    );
}
int delay(int number_of_milli_seconds)
{
    // Converting time into milli_seconds

    int clocks = 1000 * number_of_milli_seconds;

    // Storing start time

    clock_t start_time = clock();

    // looping till required time is not achieved

    while (clock() < start_time + clocks)
        ;
}

void read()
{
    // sensor_input = digital_read(0);
    int sensor_input = sensor_data();
   // int output;
    if (sensor_input == 1)
    {
        if (firstClap == 0)
        {
            firstClap = 1;
            firstClaptime = clock();
        }
        else if (firstClap == 1 && secondClap == 0)
        {
            if (clock() - firstClaptime < DIFF_MAX)
            {
                secondClap = 1;
                output(1);
                // digitalWrite(3, output);
            }
            else
            {
                firstClap = 0;
            }
        }
        else if (firstClap == 1 && secondClap == 1)
        {
            if (offClap1 == 0)
            {
                offClap1 = 1;
                offClap1time = clock();
            }
            else if (offClap1 == 1)
            {
                if (clock() - offClap1time < DIFF_MAX)
                {
                    firstClap = 0;
                    secondClap = 0;
                    output(0); // digitalwrite(3,output);
                }
                offClap1 = 0;
            }
        }
        delay(200);
    }

    if (firstClap == 1 && secondClap == 0 && (clock() - firstClaptime > DIFF_MAX))
    {
        firstClap = 0;
    }
    if (offClap1 == 1 && (clock() - offClap1time > DIFF_MAX))
    {
        offClap1 = 0;
    }
}

int main()
{
    while (1)
    {
        read();
    }

    return (0);
}
