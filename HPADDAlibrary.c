/*
 * HPADDAlibrary.c
 * This program is based on an official "ads1256_test.c".
 * This program is released under the MIT license.
 * Shujima 2018
 */

#include <bcm2835.h>  
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <math.h>
#include <errno.h>

// Globals
const uint8_t AD_SPI_CS = RPI_GPIO_P1_15;
const uint8_t DA_SPI_CS = RPI_GPIO_P1_16;
const uint8_t AD_DRDY = RPI_GPIO_P1_11;
const uint8_t AD_RESET = RPI_GPIO_P1_12;


// Enums

typedef enum {FALSE = 0, TRUE = !FALSE} bool;

/* gain channel */
typedef enum
{
	ADS1256_GAIN_1			= (0),	/* GAIN   1 */
	ADS1256_GAIN_2			= (1),	/*GAIN   2 */
	ADS1256_GAIN_4			= (2),	/*GAIN   4 */
	ADS1256_GAIN_8			= (3),	/*GAIN   8 */
	ADS1256_GAIN_16			= (4),	/* GAIN  16 */
	ADS1256_GAIN_32			= (5),	/*GAIN    32 */
	ADS1256_GAIN_64			= (6),	/*GAIN    64 */
}ADS1256_GAIN_E;

/* Sampling speed choice*/
/* 
	11110000 = 30,000SPS (default)
	11100000 = 15,000SPS
	11010000 = 7,500SPS
	11000000 = 3,750SPS
	10110000 = 2,000SPS
	10100001 = 1,000SPS
	10010010 = 500SPS
	10000010 = 100SPS
	01110010 = 60SPS
	01100011 = 50SPS
	01010011 = 30SPS
	01000011 = 25SPS
	00110011 = 15SPS
	00100011 = 10SPS
	00010011 = 5SPS
	00000011 = 2.5SPS
*/
typedef enum
{
	ADS1256_30000SPS = 0,
	ADS1256_15000SPS,
	ADS1256_7500SPS,
	ADS1256_3750SPS,
	ADS1256_2000SPS,
	ADS1256_1000SPS,
	ADS1256_500SPS,
	ADS1256_100SPS,
	ADS1256_60SPS,
	ADS1256_50SPS,
	ADS1256_30SPS,
	ADS1256_25SPS,
	ADS1256_15SPS,
	ADS1256_10SPS,
	ADS1256_5SPS,
	ADS1256_2d5SPS,

	ADS1256_DRATE_MAX
}ADS1256_DRATE_E;

#define ADS1256_DRAE_COUNT = 15;

typedef struct
{
	ADS1256_GAIN_E Gain;		/* GAIN  */
	ADS1256_DRATE_E DataRate;	/* DATA output  speed*/
	int32_t AdcNow[8];			/* ADC  Conversion value */
	uint8_t Channel;			/* The current channel*/
	uint8_t ScanMode;	/*Scanning mode,   0  Single-ended input  8 channel�� 1 Differential input  4 channel*/
}ADS1256_VAR_T;



/*Register definition Table 23. Register Map --- ADS1256 datasheet Page 30*/
enum
{
	/*Register address, followed by reset the default values */
	REG_STATUS = 0,	// x1H
	REG_MUX    = 1, // 01H
	REG_ADCON  = 2, // 20H
	REG_DRATE  = 3, // F0H
	REG_IO     = 4, // E0H
	REG_OFC0   = 5, // xxH
	REG_OFC1   = 6, // xxH
	REG_OFC2   = 7, // xxH
	REG_FSC0   = 8, // xxH
	REG_FSC1   = 9, // xxH
	REG_FSC2   = 10, // xxH
};

/* Command definition TTable 24. Command Definitions --- ADS1256 datasheet Page 34 */
enum
{
	CMD_WAKEUP  = 0x00,	// Completes SYNC and Exits Standby Mode 0000  0000 (00h)
	CMD_RDATA   = 0x01, // Read Data 0000  0001 (01h)
	CMD_RDATAC  = 0x03, // Read Data Continuously 0000   0011 (03h)
	CMD_SDATAC  = 0x0F, // Stop Read Data Continuously 0000   1111 (0Fh)
	CMD_RREG    = 0x10, // Read from REG rrr 0001 rrrr (1xh)
	CMD_WREG    = 0x50, // Write to REG rrr 0101 rrrr (5xh)
	CMD_SELFCAL = 0xF0, // Offset and Gain Self-Calibration 1111    0000 (F0h)
	CMD_SELFOCAL= 0xF1, // Offset Self-Calibration 1111    0001 (F1h)
	CMD_SELFGCAL= 0xF2, // Gain Self-Calibration 1111    0010 (F2h)
	CMD_SYSOCAL = 0xF3, // System Offset Calibration 1111   0011 (F3h)
	CMD_SYSGCAL = 0xF4, // System Gain Calibration 1111    0100 (F4h)
	CMD_SYNC    = 0xFC, // Synchronize the A/D Conversion 1111   1100 (FCh)
	CMD_STANDBY = 0xFD, // Begin Standby Mode 1111   1101 (FDh)
	CMD_RESET   = 0xFE, // Reset to Power-Up Values 1111   1110 (FEh)
};


ADS1256_VAR_T g_tADS1256;
static const uint8_t s_tabDataRate[ADS1256_DRATE_MAX] =
{
	0xF0,		/*reset the default values  */
	0xE0,
	0xD0,
	0xC0,
	0xB0,
	0xA1,
	0x92,
	0x82,
	0x72,
	0x63,
	0x53,
	0x43,
	0x33,
	0x20,
	0x13,
	0x03
};








//#####################################################################
//
//  Prototype Declaration of Functions 
//
//#####################################################################

// Common
void  delay_us(uint64_t micros);
int initHPADDAboard();
void closeHPADDAboard();

// DA
void DAC8532_Write( int dac_channel , unsigned int val);
unsigned int DAC8532_Volt2Value( double volt , double volt_ref);

void DAC8532_SetCS(char b); //(Private)

// Get AD
int32_t ADS1256_GetAdc(uint8_t _ch);
int32_t ADS1256_GetAdcDiff(uint8_t positive_no , uint8_t negative_no );

// Print AD
void ADS1256_PrintAllValue();
void ADS1256_PrintAllValueDiff();
void ADS1256_PrintAllReg();
double ADS1256_Value2Volt(uint32_t value , double vref);

// Connection to AD
static int32_t ADS1256_ReadData(void);


// AD settings
void ADS1256_CfgADC(ADS1256_GAIN_E _gain, ADS1256_DRATE_E _drate);
uint8_t ADS1256_ReadChipID(void);
static void ADS1256_WriteReg(uint8_t _RegID, uint8_t _RegValue);
static uint8_t ADS1256_ReadReg(uint8_t _RegID);
static void ADS1256_WriteCmd(uint8_t _cmd);

// AD others (Private)
static void ADS1256_DelayDATA(void);
void ADS1256_WaitDRDY(void);
void ADS1256_ChangeMUX(int8_t positive_no , int8_t negative_no );
void ADS1256_SetCS(char b);

// AD bottom layer of connection (Private)
static void ADS1256_Send8Bit(uint8_t _data);
static uint8_t ADS1256_Recive8Bit(void);



/*
*********************************************************************************************************
*	name: delay_us
*	function:  delay
*	parameter: micros : [us]
*	The return value: NULL
*********************************************************************************************************
*/
void  delay_us(uint64_t micros)
{
		bcm2835_delayMicroseconds (micros);
}


/*
*********************************************************************************************************
*	name: initHPADDAboard
*	function:  init ADS1256 & DAC8532 
*	parameter: NULL
*	The return value:  0:successful 1:Abnormal
*********************************************************************************************************
*/
int initHPADDAboard()
{
    int initstate = bcm2835_init();
    if (initstate < 0) { perror("Couldn't start bcm2835 lib."); return -1;}
    bcm2835_spi_begin();
    bcm2835_spi_setBitOrder(BCM2835_SPI_BIT_ORDER_LSBFIRST );
    bcm2835_spi_setDataMode(BCM2835_SPI_MODE1);
    bcm2835_spi_setClockDivider(BCM2835_SPI_CLOCK_DIVIDER_1024); 
    bcm2835_gpio_fsel( AD_SPI_CS , BCM2835_GPIO_FSEL_OUTP);
    bcm2835_gpio_write( AD_SPI_CS , HIGH);
    bcm2835_gpio_fsel( DA_SPI_CS , BCM2835_GPIO_FSEL_OUTP);
    bcm2835_gpio_write( DA_SPI_CS , HIGH);
    bcm2835_gpio_fsel( AD_DRDY , BCM2835_GPIO_FSEL_INPT );
    bcm2835_gpio_set_pud( AD_DRDY , BCM2835_GPIO_PUD_UP );

    //ADS1256_WriteReg(REG_MUX,0x01);
    //ADS1256_WriteReg(REG_ADCON,0x20);
   	// ADS1256_CfgADC(ADS1256_GAIN_1, ADS1256_15SPS);	
	
	return 0;
}

/*
*********************************************************************************************************
*	name: closeHPADDAboard
*	function:  close SPI, ADS1256 & DAC8532 
*	parameter: NULL
*	The return value:  NULL
*********************************************************************************************************
*/
void closeHPADDAboard()
{
    bcm2835_spi_end();
    bcm2835_close();
}





//#####################################################################
//  DA Functions




/*
*********************************************************************************************************
*	name: DAC8532_Write
*	function:  change an output of DAC8532 to target value 
*	parameter:  dac_channel : channel of DAC8532 (0 or 1)
				val : output value to DAC8532 ( 0 - 65536 ) , please use DAC8532_Volt2Value function
*	The return value:  NULL
*********************************************************************************************************
*/
void DAC8532_Write( int dac_channel , unsigned int val)
{
    bcm2835_gpio_write(DA_SPI_CS , 0);
    if(dac_channel == 0)
    {
        bcm2835_spi_transfer(0x30);
    }
    else if(dac_channel == 1)
    {
        bcm2835_spi_transfer(0x34);
    }

    bcm2835_spi_transfer( (val & 0xff00) >> 8 ); //上位8ビットの送信
    bcm2835_spi_transfer( val & 0x00ff ); //下位8ビットの送信

    bcm2835_gpio_write(DA_SPI_CS , 1);
}

/*
*********************************************************************************************************
*	name: DAC8532_Volt2Value
*	function:  convert value from volt to 16bit value ( 0 - 65535 )
*	parameter:  volt : target volt [v] ( 0 - 5.0 )
				volt_ref : reference volt [v] ( 3.3 or 5.0 )
*	The return value:  output value to DAC8532 ( 0 - 65535 )
*********************************************************************************************************
*/
unsigned int DAC8532_Volt2Value( double volt , double volt_ref)
{
    return ( unsigned int ) ( 65536 * volt / volt_ref );
}

/*
*********************************************************************************************************
*	name: DAC8532_SetCS
*	function:  set SPI CS pin value of DAC8532 
*	parameter: b : bool value for SPI CS status (0 : connection start , 1: connection end)
*	The return value:  NULL
*********************************************************************************************************
*/
void DAC8532_SetCS(char b)
{
	if(b)
	{
		bcm2835_gpio_write(DA_SPI_CS,HIGH);
	}
	else
	{
		bcm2835_gpio_write(DA_SPI_CS,LOW);
	}
}




//#####################################################################
//  AD Functions




/*
*********************************************************************************************************
*	name: ADS1256_GetAdc
*	function: read ADC value
*	parameter:  channel number 0--7
*	The return value:  ADC vaule (signed number)
*********************************************************************************************************
*/
int32_t ADS1256_GetAdc(uint8_t ch)
{
	ADS1256_ChangeMUX(ch , -1);
	ADS1256_WaitDRDY();
	return ADS1256_ReadData();
}

/*
*********************************************************************************************************
*	name: ADS1256_GetAdc
*	function: read ADC value
*	parameter:  positive_no : input port no of positive side (0 - 7)
				negative_no : input port no of negative side (0 - 7)
*	The return value:  ADC vaule (signed number)
*********************************************************************************************************
*/
int32_t ADS1256_GetAdcDiff(uint8_t positive_no , uint8_t negative_no )
{
	ADS1256_ChangeMUX(positive_no , negative_no);
	ADS1256_WaitDRDY();
	return ADS1256_ReadData();
}

/*
*********************************************************************************************************
*	name: ADS1256_PrintAllValue
*	function:  print 8 values of ADS1256 input
*	parameter: NULL
*	The return value:  NULL
*********************************************************************************************************
*/
void ADS1256_PrintAllValue()
{
	uint8_t i;
	int32_t adval;
	for (i = 0 ; i < 8 ; i ++)
	{
		adval = ADS1256_GetAdc(i);
		printf("%d : %d \t(%f[V])\n",i ,adval, ADS1256_Value2Volt(adval,  5.0 ) );
		//ADS1256_PrintAllReg();
	}
}

/*
*********************************************************************************************************
*	name: ADS1256_PrintAllValueDiff
*	function:  print 4 values of ADS1256 differential input
*	parameter: NULL
*	The return value:  NULL
*********************************************************************************************************
*/
void ADS1256_PrintAllValueDiff()
{
	uint8_t i;
	int32_t adval;
	for (i = 0 ; i < 4 ; i ++)
	{
		adval = ADS1256_GetAdcDiff( 2 * i , 2 * i + 1 );
		printf("%d-%d : %d \t(%f[V])\n",2 * i, 2 * i + 1 ,adval, ADS1256_Value2Volt(adval,  5.0 ) );
		//ADS1256_PrintAllReg();
	}
}

/*
*********************************************************************************************************
*	name: ADS1256_PrintAllReg
*	function:  print all regulator of ADS1256
*	parameter: NULL
*	The return value:  NULL
*********************************************************************************************************
*/
void ADS1256_PrintAllReg()
{
	const char regname[11][20] = {"STATUS", "MUX   ", "ADCON ", "DRATE ", "IO    ",
							"OFC0  ", "OFC1  ", "OFC2  ", "FSC0  ", "FSC1  ", "FSC2  "};
	uint8_t i;
	for (i = 0 ; i < 11 ; i ++)
	{
		printf("%s : %x\n", regname[i] ,ADS1256_ReadReg(i));
	}
}

/*
*********************************************************************************************************
*	name: ADS1256_Value2Volt
*	function:  convert ADC output value to volt [V] 
*	parameter: value :  output value ( 0 - 0x7fffff )
						reference voltage [V] ( 3.3 or 5.0 )
*	The return value:  ADC voltage [V]
*********************************************************************************************************
*/
double ADS1256_Value2Volt(uint32_t value , double vref)
{
	return value * 1.0 / 0x7fffff * vref;
}

/*
*********************************************************************************************************
*	name: ADS1256_ReadData
*	function: read ADC value
*	parameter: NULL
*	The return value:  NULL
*********************************************************************************************************
*/
static int32_t ADS1256_ReadData(void)
{
	uint32_t read = 0;
    static uint8_t buf[3];
	DAC8532_SetCS(HIGH);
	ADS1256_SetCS(LOW);	/* SPI   cs = 0 */

	ADS1256_Send8Bit(CMD_RDATA);	/* read ADC command  */

	ADS1256_DelayDATA();	/*delay time  */

	/*Read the sample results 24bit*/
    buf[0] = ADS1256_Recive8Bit();
    buf[1] = ADS1256_Recive8Bit();
    buf[2] = ADS1256_Recive8Bit();

    read = ((uint32_t)buf[0] << 16) & 0x00FF0000;
    read |= ((uint32_t)buf[1] << 8);  /* Pay attention to It is wrong   read |= (buf[1] << 8) */
    read |= buf[2];

	ADS1256_SetCS(HIGH);	/* SPIƬѡ = 1 */

	/* Extend a signed number*/
    if (read & 0x800000)
    {
	    read |= 0xFF000000;
    }

	return (int32_t)read;
}

/*
*********************************************************************************************************
*	name: ADS1256_CfgADC
*	function: The configuration parameters of ADC, gain and data rate
*	parameter: _gain:gain 1-64
*                      _drate:  data  rate
*	The return value: NULL
*********************************************************************************************************
*/
void ADS1256_CfgADC(ADS1256_GAIN_E _gain, ADS1256_DRATE_E _drate)
{
	g_tADS1256.Gain = _gain;
	g_tADS1256.DataRate = _drate;

	ADS1256_WaitDRDY();

	{
		uint8_t buf[4];		/* Storage ads1256 register configuration parameters */

		/*Status register define
			Bits 7-4 ID3, ID2, ID1, ID0  Factory Programmed Identification Bits (Read Only)

			Bit 3 ORDER: Data Output Bit Order
				0 = Most Significant Bit First (default)
				1 = Least Significant Bit First
			Input data  is always shifted in most significant byte and bit first. Output data is always shifted out most significant
			byte first. The ORDER bit only controls the bit order of the output data within the byte.

			Bit 2 ACAL : Auto-Calibration
				0 = Auto-Calibration Disabled (default)
				1 = Auto-Calibration Enabled
			When Auto-Calibration is enabled, self-calibration begins at the completion of the WREG command that changes
			the PGA (bits 0-2 of ADCON register), DR (bits 7-0 in the DRATE register) or BUFEN (bit 1 in the STATUS register)
			values.

			Bit 1 BUFEN: Analog Input Buffer Enable
				0 = Buffer Disabled (default)
				1 = Buffer Enabled

			Bit 0 DRDY :  Data Ready (Read Only)
				This bit duplicates the state of the DRDY pin.

			ACAL=1  enable  calibration
		*/
		//buf[0] = (0 << 3) | (1 << 2) | (1 << 1);//enable the internal buffer
        buf[0] = (0 << 3) | (1 << 2) | (0 << 1);  // The internal buffer is prohibited

        //ADS1256_WriteReg(REG_STATUS, (0 << 3) | (1 << 2) | (1 << 1));

		buf[1] = 0x08;	

		/*	ADCON: A/D Control Register (Address 02h)
			Bit 7 Reserved, always 0 (Read Only)
			Bits 6-5 CLK1, CLK0 : D0/CLKOUT Clock Out Rate Setting
				00 = Clock Out OFF
				01 = Clock Out Frequency = fCLKIN (default)
				10 = Clock Out Frequency = fCLKIN/2
				11 = Clock Out Frequency = fCLKIN/4
				When not using CLKOUT, it is recommended that it be turned off. These bits can only be reset using the RESET pin.

			Bits 4-3 SDCS1, SCDS0: Sensor Detect Current Sources
				00 = Sensor Detect OFF (default)
				01 = Sensor Detect Current = 0.5 �� A
				10 = Sensor Detect Current = 2 �� A
				11 = Sensor Detect Current = 10�� A
				The Sensor Detect Current Sources can be activated to verify  the integrity of an external sensor supplying a signal to the
				ADS1255/6. A shorted sensor produces a very small signal while an open-circuit sensor produces a very large signal.

			Bits 2-0 PGA2, PGA1, PGA0: Programmable Gain Amplifier Setting
				000 = 1 (default)
				001 = 2
				010 = 4
				011 = 8
				100 = 16
				101 = 32
				110 = 64
				111 = 64
		*/
		buf[2] = (0 << 5) | (0 << 3) | (_gain << 0);
		//ADS1256_WriteReg(REG_ADCON, (0 << 5) | (0 << 2) | (GAIN_1 << 1));	/*choose 1: gain 1 ;input 5V/
		buf[3] = s_tabDataRate[_drate];	// DRATE_10SPS;	

		DAC8532_SetCS(HIGH);
		ADS1256_SetCS(LOW);	/* SPIƬѡ = 0 */
		ADS1256_Send8Bit(CMD_WREG | 0);	/* Write command register, send the register address */
		ADS1256_Send8Bit(0x03);			/* Register number 4,Initialize the number  -1*/

		ADS1256_Send8Bit(buf[0]);	/* Set the status register */
		ADS1256_Send8Bit(buf[1]);	/* Set the input channel parameters */
		ADS1256_Send8Bit(buf[2]);	/* Set the ADCON control register,gain */
		ADS1256_Send8Bit(buf[3]);	/* Set the output rate */

		ADS1256_SetCS(HIGH);	/* SPI  cs = 1 */
	}

	delay_us(50);
}

/*
*********************************************************************************************************
*	name: ADS1256_WriteReg
*	function: Write the corresponding register
*	parameter: _RegID: register  ID
*			 _RegValue: register Value
*	The return value: NULL
*********************************************************************************************************
*/
static void ADS1256_WriteReg(uint8_t _RegID, uint8_t _RegValue)
{
	DAC8532_SetCS(HIGH);
	ADS1256_SetCS(LOW);	/* SPI  cs  = 0 */
	ADS1256_Send8Bit(CMD_WREG | _RegID);	/*Write command register */
	ADS1256_Send8Bit(0x00);		/*Write the register number */

	ADS1256_Send8Bit(_RegValue);	/*send register value */
	ADS1256_SetCS(HIGH);	/* SPI   cs = 1 */
}

/*
*********************************************************************************************************
*	name: ADS1256_ReadReg
*	function: Read  the corresponding register
*	parameter: _RegID: register  ID
*	The return value: read register value
*********************************************************************************************************
*/
static uint8_t ADS1256_ReadReg(uint8_t _RegID)
{
	uint8_t read;

	DAC8532_SetCS(HIGH);
	ADS1256_SetCS(LOW);	/* SPI  cs  = 0 */
	ADS1256_Send8Bit(CMD_RREG | _RegID);	/* Write command register */
	ADS1256_Send8Bit(0x00);	/* Write the register number */

	ADS1256_DelayDATA();	/*delay time */

	read = ADS1256_Recive8Bit();	/* Read the register values */
	ADS1256_SetCS(HIGH);	/* SPI   cs  = 1 */

	return read;
}

/*
*********************************************************************************************************
*	name: ADS1256_WriteCmd
*	function: Sending a single byte order
*	parameter: _cmd : command
*	The return value: NULL
*********************************************************************************************************
*/
static void ADS1256_WriteCmd(uint8_t _cmd)
{
	DAC8532_SetCS(HIGH);
	ADS1256_SetCS(LOW);	/* SPI   cs = 0 */
	ADS1256_Send8Bit(_cmd);
	ADS1256_SetCS(HIGH);	/* SPI  cs  = 1 */
}

/*
*********************************************************************************************************
*	name: ADS1256_ReadChipID
*	function: Read the chip ID
*	parameter: _cmd : NULL
*	The return value: four high status register
*********************************************************************************************************
*/
uint8_t ADS1256_ReadChipID(void)
{
	uint8_t id;

	ADS1256_WaitDRDY();
	id = ADS1256_ReadReg(REG_STATUS);
	return (id >> 4);
}






//#####################################################################
//  Private Functions




/*
*********************************************************************************************************
*	name: ADS1256_DelayDATA
*	function: delay
*	parameter: NULL
*	The return value: NULL
*********************************************************************************************************
*/
static void ADS1256_DelayDATA(void)
{
	/*
		Delay from last SCLK edge for DIN to first SCLK rising edge for DOUT: RDATA, RDATAC,RREG Commands
		min  50   CLK = 50 * 0.13uS = 6.5uS
	*/
	delay_us(10);	/* The minimum time delay 6.5us */
}

/*
*********************************************************************************************************
*	name: ADS1256_WaitDRDY
*	function: delay time  wait for automatic calibration
*	parameter:  NULL
*	The return value:  NULL
*********************************************************************************************************
*/
void ADS1256_WaitDRDY(void)
{
	uint32_t i;

	for (i = 0; i < 1000000; i++)
	{
		if (bcm2835_gpio_lev(AD_DRDY)==0)
		{
			return;
		}
		delay_us(1);
	}

	perror("ADS1256_WaitDRDY() Time Out ...\r\n");	
}

/*
*********************************************************************************************************
*	name: ADS1256_ChangeMUX
*	function:  set ADS1256 MUX for changing input of ADC 
*	parameter: 	positive_no :input port no of positive side (0 - 7 or -1)
							 Nomally it should be set to 0 - 7
							 When set to -1 , AGND becomes positive input
				negative_no :input port no of negative side (-1 or 0 - 7)
							 Nomally it should be set to -1
							 When use Differential Input, it should be set to 0 - 7
*	The return value:  NULL
*********************************************************************************************************
*/
void ADS1256_ChangeMUX(int8_t positive_no , int8_t negative_no )
{
	/*
	|---------------------------------------------------------------|
	| ADS1256 / Register / MUX (0x01)                               |
	|---------------------------------------------------------------|
	| Bit 7 | Bit 6 | Bit 5 | Bit 4 | Bit 3 | Bit 2 | Bit 1 | Bit 0 |
	|---------------------------------------------------------------|
	| Bit 7-4 positive side input   | Bit 3-0 negative side input   |
	| 0000 to 0111 : AIN0 to AIN7   | 0000 to 0111 : AIN0 to AIN7   |
	| 1xxx : AINCOM (AGND)          | 1xxx : AINCOM (AGND)          |
	|---------------------------------------------------------------|
	*/

	uint8_t positive_common = 0;	//Bit 7
	uint8_t negative_common = 0;	//Bit 3
	if (positive_no < 0 || positive_no > 7)
	{
		positive_common = 1;
		if ( positive_no != -1 )perror("Illegal Input\n");
	}
	if (negative_no < 0 || negative_no > 7)
	{
		negative_common = 1;
		if ( negative_no != -1 )perror("Illegal Input\n");
	}

	ADS1256_WriteReg(REG_MUX,(positive_common << 7) | ((positive_no & 7) << 4) | (negative_common << 3) | ((negative_no & 7)) );
	delay_us(5);

	ADS1256_WriteCmd(CMD_SYNC);
	delay_us(5);

	ADS1256_WriteCmd(CMD_WAKEUP);
	delay_us(25);
}

/*
*********************************************************************************************************
*	name: ADS1256_SetCS
*	function:  set SPI CS pin value of ADS1256 
*	parameter: b : bool value for SPI CS status (0 : connection start , 1: connection end)
*	The return value:  NULL
*********************************************************************************************************
*/
void ADS1256_SetCS(char b)
{
	if(b)
	{
		bcm2835_gpio_write(AD_SPI_CS,HIGH);
	}
	else
	{
		bcm2835_gpio_write(AD_SPI_CS,LOW);
	}
}

/*
*********************************************************************************************************
*	name: ADS1256_Send8Bit
*	function: SPI bus to send 8 bit data
*	parameter: _data:  data
*	The return value: NULL
*********************************************************************************************************
*/
static void ADS1256_Send8Bit(uint8_t _data)
{

	delay_us(2);
	bcm2835_spi_transfer(_data);
}

/*
*********************************************************************************************************
*	name: ADS1256_Recive8Bit
*	function: SPI bus receive function
*	parameter: NULL
*	The return value: NULL
*********************************************************************************************************
*/
static uint8_t ADS1256_Recive8Bit(void)
{
	uint8_t read = 0;
	read = bcm2835_spi_transfer(0xff);
	return read;
}












