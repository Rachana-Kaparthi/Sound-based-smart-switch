#include <stdio.h>
int bulb ;
int indicator = 0;

//void output_indicator(int value);
//void output_bulb(int value);
//int sensor_data();
void read(int sensor_input);
void delay(int seconds);

int main()
{
   int i = 0;
    int j = 0;
    int si = 0;
    while (j < 100)
    {
        if(i == 10) si = 1;
        read(si);
        if(i == 10) { si = 0; i = 0;}
        i++;
        j++;
    }

    return (0);
}



void delay(int seconds) {
    int i, j;
    for (i = 0; i < seconds; i++) {
        for (j = 0; j < 1000000; j++) {
            // Adding a loop inside to create some delay as sound may last for some time
        }
    }
}

void read(int sensor_input)
{
    // sensor_input = digital_read(0);
    printf("sensorinput value = %d\n", sensor_input);

    if (sensor_input == 1)
    {
        
        indicator = 1;
        //digitalWrite(indicator, HIGH);
        //output_indicator(1); // sound is not detected when this led is high
        printf("indicator value %d bulb value %d\n", indicator, bulb);

        if(bulb == 0)
        {
            bulb = 1;
            //output_bulb(0);
            printf("setting output value  to %d\n", bulb);
        }
        else
        {
            bulb = 0;
            //output_bulb(1);
            printf("setting output value  to %d\n", bulb);
        }
        delay(1000);// waiting for approx 2 sec before sensing the input so that the present sound subsides
        indicator = 0; // sound is only detected when this led goes low
        printf("setting indicator value back to %d after a delay to sense the sound again \n", indicator);
    }

    
}


