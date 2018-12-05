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

## Use for DA
### void setCS_DAC8532(char b)
*	function:  set SPI CS pin value of DAC8532 
*	parameter: b : bool value for SPI CS status (0 : connection start , 1: connection end)
*	The return value:  NULL

### void writeDAC8532( int dac_channel , unsigned int val)
*	function:  change an output of DAC8532 to target value 
*	parameter:  dac_channel : channel of DAC8532 (0 or 1)
				val : output value to DAC8532 ( 0 - 65536 ) , please use volt2DAC8532val function
*	The return value:  NULL

### unsigned int volt2DAC8532val( double volt , double volt_ref)
*	function:  convert value from volt to 16bit value ( 0 - 65535 )
*	parameter:  volt : target volt [v] ( 0 - 5.0 )
				volt_ref : reference volt [v] ( 3.3 or 5.0 )
*	The return value:  output value to DAC8532 ( 0 - 65535 )

## Use for AD