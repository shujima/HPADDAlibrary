# HPADDAlibrary
C Library for High-Precision AD/DA Board

This library includes High-Precision AD/DA Board official files(https://www.waveshare.com/wiki/File:High-Precision-AD-DA-Board-Code.7z).
I got permission from an administrator of the official for publication.


# Required
* Hardware
	* Raspberry Pi
	* High-Precision AD/DA Board
* Library
	* bcm2835 library

# Usage

```shell
cd ./HPADDAlibrary
make
sudo ./Test
```

# Functions

## Use for both AD and DA

### int initHPADDAboard()
*	function:  init ADS1256 & DAC8532 
*	parameter: NULL
*	The return value:  0:successful 1:Abnormal

### void closeHPADDAboard()
*	function:  close SPI, ADS1256 & DAC8532 
*	parameter: NULL
*	The return value:  NULL

### void delay_us(uint64_t micros)
*	function:  wait[us]
*	parameter:
	* [us]
*	The return value:  NULL

## Use for DA

### void writeDAC8532( int dac_channel , unsigned int val)
*	function:  change an output of DAC8532 to target value 
*	parameter:
	  * dac_channel : channel of DAC8532 (0 or 1)
	  * val : output value to DAC8532 ( 0 - 65536 ) , please use volt2DAC8532val function
*	The return value:  NULL

### unsigned int voltToDAC8532val( double volt , double volt_ref)
*	function:   convert value from volt to 16bit value ( 0 - 65535 )
*	parameter:  volt : target volt [v] ( 0 - 5.0 )
	  * volt_ref : reference volt [v] ( 3.3 or 5.0 )
*	The return value:  output value to DAC8532 ( 0 - 65535 )

## Use for AD

### int32_t ADS1256_GetAdc(uint8_t _ch);
*	function: read ADC value
*	parameter:  channel number 0--7
*	The return value:  ADC vaule (signed number)

### int32_t ADS1256_GetAdcDiff(uint8_t positive_no , uint8_t negative_no );
*	function: read ADC value
*	parameter:  positive_no : input port no of positive side (0 - 7)
	  * negative_no : input port no of negative side (0 - 7)
*	The return value:  ADC vaule (signed number)

### double ADS1256_ValueToVolt(uint32_t value , double vref);
*	function:  convert ADC output value to volt [V] 
*	parameter: value :  output value ( 0 - 0x7fffff )
	  * reference voltage [V] ( 3.3 or 5.0 )
*	The return value:  ADC voltage [V]

## Use for AD (Print)

### void ADS1256_PrintAllValue();
*	function:  print 8 values of ADS1256 input
*	parameter: NULL
*	The return value:  NULL

### void ADS1256_PrintAllValueDiff();
*	name: ADS1256_PrintAllValueDiff
*	function:  print 4 values of ADS1256 differential input
*	parameter: NULL
*	The return value:  NULL

### void ADS1256_PrintAllReg();
*	function:  print all register of ADS1256
*	parameter: NULL
*	The return value:  NULL

## Use for AD (Settings)

### double ADS1256_SetSampleRate(double rate)
*	function: set sampling rate of ADS1256
*	parameter: 
	* rate : [samples per second] (2.5 - 30000)
		* This rate is for one input. It becomes later in the case of multi input.
*	The return value: Actual set value

### int ADS1256_SetGain(int gain)
*	function: set Gain of ADS1256
*	parameter: 
	* rate : Gain (1 - 64)
*	The return value: Actual set value

### uint8_t ADS1256_ReadChipID(void);
*	function: Read the chip ID
*	parameter: 
	* _cmd : NULL
*	The return value: four high status register

### static void ADS1256_WriteReg(uint8_t _RegID, uint8_t _RegValue);
*	function: Write the corresponding register
*	parameter: 
	  * _RegID: register  ID
	  * _RegValue: register Value
*	The return value: NULL

### static uint8_t ADS1256_ReadReg(uint8_t _RegID);
*	function: Read  the corresponding register
*	parameter: 
	  * _RegID: register  ID
*	The return value: read register value

### static void ADS1256_WriteCmd(uint8_t _cmd);
*	function: Sending a single byte order
*	parameter:
	  * _cmd : command
*	The return value: NULL

## Use for AD (Private Functions)

### void setCS_DAC8532(char b)
*	function:  set SPI CS pin value of DAC8532 
*	parameter: 
	* b : bool value for SPI CS status (0 : connection start , 1: connection end)
*	The return value:  NULL

### static int32_t ADS1256_ReadData(void);
*	function: read ADC value
*	parameter: NULL
*	The return value:  NULL

### static void ADS1256_DelayDATA(void);
*	function: delay
*	parameter: NULL
*	The return value: NULL

### void ADS1256_WaitDRDY(void);
*	function: delay time  wait for automatic calibration
*	parameter:  NULL
*	The return value:  NULL

### void ADS1256_ChangeMUX(int8_t positive_no , int8_t negative_no );
*	function:  set ADS1256 MUX for changing input of ADC 
*	parameter: 
	  * positive_no :input port no of positive side (0 - 7 or -1)
		    * Nomally it should be set to 0 - 7. When set to -1 , AGND becomes positive input.
	  * negative_no :input port no of negative side (-1 or 0 - 7)
		    * Nomally it should be set to -1. When use Differential Input, it should be set to 0 - 7.
*	The return value: NULL

### void ADS1256_SetCS(char b);
*	function:  set SPI CS pin value of ADS1256 
*	parameter:
	* b : bool value for SPI CS status (0 : connection start , 1: connection end)
*	The return value:  NULL

### static void ADS1256_Send8Bit(uint8_t _data);
*	function: SPI bus to send 8 bit data
*	parameter:
	* _data:  data
*	The return value: NULL

### static uint8_t ADS1256_Recive8Bit(void);
*	function: SPI bus receive function
*	parameter: NULL
*	The return value: NULL

