#include <stdio.h>
#include <math.h>
#include "HPADDAboard.h"

int  main()
{
    char c = '0';
    printf("High-Precision AD/DA Board Test Program\n");
    printf("What do you want to do? (please return after input)\n");
    printf("[1]: AD-All Channel Test\n");
    printf("[2]: DA-All Channel Test\n");
    printf("your input = ");

    c = getchar();

    if(c == '1')
    {
        uint8_t id;

        initHPADDAboard();
        checkADS1256ID();

        while(1)
        {
            printAllADval();
            bsp_DelayUS(100000);
        }

        closeHPADDAboard();
    }
    else if (c == '2')
    {
        int i = 0;
        initHPADDAboard();
        printf("DACstart\n");

        while(1)
        {
            writeDAC8532( 0 , volt2DAC8532val( fabs( 5.0 * sin( i / 10.0 ) ) , 5.0 ) );
            writeDAC8532( 1 , volt2DAC8532val( fabs( 5.0 * cos( i / 10.0 ) ) , 5.0 ) );
            printf("DAC0 = %f [V], DAC1 = %f [V]\n" ,  5.0 * fabs( sin( i / 10.0 ) ) , 5.0 * fabs( cos( i / 10.0 ) ));
            i++;
            bsp_DelayUS(100000);
        } 
    }

	
    return 0;
}