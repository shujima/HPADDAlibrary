#include <stdio.h>
#include <math.h>
#include "HPADDAlibrary.h"

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
        initHPADDAboard();
        uint8_t id;
        id = ADS1256_ReadChipID();
        printf("ID=\r\n");  
        if (id != 3)
        {
            printf("Error, ADS1256 Chip ID = 0x%d\r\n", (int)id);
        }
        else
        {
            printf("Ok, ADS1256 Chip ID = 0x%d\r\n", (int)id);
        }
        ADS1256_CfgADC(ADS1256_GAIN_1, ADS1256_15SPS);
        //ADS1256_CfgADC(ADS1256_GAIN_1, ADS1256_100SPS);
        // ADS1256_StartScan(0);
        while(1)
        {
            //printAllADval();
            ADS1256_PrintAllValue();
            //ADS1256_PrintAllReg();
            //bsp_DelayUS(100000);
            delay_us(100000);
            printf("\033[%dA",8);
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
            DAC8532_Write( 0 , DAC8532_Volt2Value( fabs( 5.0 * sin( i / 10.0 ) ) , 5.0 ) );
            DAC8532_Write( 1 , DAC8532_Volt2Value( fabs( 5.0 * cos( i / 10.0 ) ) , 5.0 ) );
            printf("DAC0 = %f [V], DAC1 = %f [V]\n" ,  5.0 * fabs( sin( i / 10.0 ) ) , 5.0 * fabs( cos( i / 10.0 ) ));
            i++;
            //bsp_DelayUS(100000);
            delay_us(100000);
        } 
    }

	
    return 0;
}