#include <stdio.h>
#include <math.h>
#include "HPADDAlibrary.h"

int  main()
{

    char c = '0';
    printf("High-Precision AD/DA Board Test Program\n");
    printf("What do you want to do? (please return after input)\n");
    printf("[1]: AD Check Chip ID\n");
    printf("[2]: AD All Channel Test (Single)\n");
    printf("[3]: AD All Channel Test (Differential)\n");
    printf("[4]: AD All Channel Test (Convination)\n");
    printf("[5]: DA All Channel Test\n");
    printf("[6]: AD to DA Test\n");
    printf("your input = ");

    c = getchar();


    switch(c)
    {

        
        case '1':
        {
            initHPADDAboard();
            uint8_t id;
            id = ADS1256_ReadChipID();
            printf("\nID=%d\r\n\n", id);  
            if (id != 3)
            {
                printf("Error, ADS1256 Chip ID = 0x%d\r\n", (int)id);
            }
            else
            {
                printf("Ok, ADS1256 Chip ID = 0x%d\r\n", (int)id);
            }
            closeHPADDAboard();
            break;
        }



        case '2':
        {
            initHPADDAboard();
            ADS1256_CfgADC(ADS1256_GAIN_1, ADS1256_15SPS);
            printf("AD Combination both Single and Differential\n");
            printf("AGND- <-.\n");
            printf("AD0+ <--|\n");
            printf("AD1+ <--|\n");
            printf("AD2+ <--|\n");
            printf("AD3+ <--|\n");
            printf("AD4+ <--|\n");
            printf("AD5+ <--|\n");
            printf("AD6+ <--|\n");
            printf("AD7+ <--'\n");
            while(1)
            {
                ADS1256_PrintAllValue();
                delay_us(100000);
                printf("\033[%dA",8);   // Go back 8 lines
            }
            closeHPADDAboard();
            break;
        }




        case '3':
        {
            initHPADDAboard();                       
            ADS1256_CfgADC(ADS1256_GAIN_1, ADS1256_15SPS);
            printf("AD Differential Input\n");
            printf("AD0+ <--.\n");
            printf("AD1- <--'\n");
            printf("AD2+ <--.\n");
            printf("AD3- <--'\n");
            printf("AD4+ <--.\n");
            printf("AD5- <--'\n");
            printf("AD6+ <--.\n");
            printf("AD7- <--'\n");
            while(1)
            {
                ADS1256_PrintAllValueDiff();
                delay_us(100000);
                printf("\033[%dA",4);   // Go back 4 lines
            }
            closeHPADDAboard();
            break;            
        }

        case '4':
        {
            initHPADDAboard();
            ADS1256_CfgADC(ADS1256_GAIN_1, ADS1256_15SPS);
            printf("AD Combination both Single and Differential\n");
            printf("AGND- <-.\n");
            printf("AD0+ <--|\n");
            printf("AD1+ <--'\n");
            printf("AD2+ <--.\n");
            printf("AD3- <--'\n");
            printf("AD4+ <--.\n");
            printf("AD5- <--'\n");
            printf("AD6+ <--.\n");
            printf("AD7- <--'\n");
            while(1)
            {
                int i;
                long val;
                // Single
                for (i = 0 ; i < 2 ; i ++)
                {
                    val = ADS1256_GetAdc(i);
                printf("%d+ <=> -AGND (Single)  : %8d\t (%f[V])\n", i , val,
                     ADS1256_Value2Volt(val, 5.0) );
                }

                // Differential
                for (i = 2 ; i < 8 ; i += 2)
                {
                    val = ADS1256_GetAdcDiff(i, i + 1);
                printf("%d+ <=> -%d (Differntial): %8d\t (%f[V])\n", i, i + 1 ,
                     val, ADS1256_Value2Volt(val, 5.0) );
                }
                delay_us(100000);
                printf("\033[%dA",5);   // Go back 5 lines
            }
        }

        case '5':
        {
            int i = 0;
            initHPADDAboard();
            printf("DAC Test (Sine Wave)\n");

            while(1)
            {
                DAC8532_Write( 0 , DAC8532_Volt2Value( fabs( 5.0 * sin( i / 10.0 ) ) , 5.0 ) );
                DAC8532_Write( 1 , DAC8532_Volt2Value( fabs( 5.0 * cos( i / 10.0 ) ) , 5.0 ) );
                printf("DAC0 = %f [V], DAC1 = %f [V]\n" ,  5.0 * fabs( sin( i / 10.0 ) ) , 5.0 * fabs( cos( i / 10.0 ) ));
                i++;
                delay_us(100000);
            } 
            break;         
        }

        case '6':
        {
            initHPADDAboard();
            ADS1256_CfgADC(ADS1256_GAIN_1, ADS1256_15SPS);
            printf("AD to DA\n");
            printf("AD0 ---> DA0\n");
            printf("AD1 ---> DA1\n");

            while(1)
            {
                int i;
                long val;
                // Read AD
                for (i = 0 ; i < 2 ; i ++)
                {
                    val = ADS1256_GetAdc(i);
                    printf("%d+ <=> -AGND (Single)  : %8d\t (%f[V])\n", i , val,
                        ADS1256_Value2Volt(val, 5.0) );
                    DAC8532_Write( i , DAC8532_Volt2Value( ADS1256_Value2Volt(val, 5.0) , 5.0 ) );
                }
                delay_us(100000);
                printf("\033[%dA",2);   // Go back 2 lines
            } 
            break;         
        }  
    }
    return 0;
}