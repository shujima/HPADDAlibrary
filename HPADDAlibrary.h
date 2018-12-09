/*
 * HPADDAlibrary.c
 * This program is based on an official "ads1256_test.c".
 * This program is released under the MIT license.
 * shujima 2018
 */
# include <stdint.h> //uint8_t, ...
// Enums

// Register definition Table 23. Register Map --- ADS1256 datasheet Page 30
enum
{   REG_STATUS = 0, REG_MUX , REG_ADCON , REG_DRATE , REG_IO ,
    REG_OFC0 , REG_OFC1 , REG_OFC2 , REG_FSC0 , REG_FSC1 , REG_FSC2

};

// Command definition TTable 24. Command Definitions --- ADS1256 datasheet Page 34
enum
{
    CMD_WAKEUP  = 0x00,    // Completes SYNC and Exits Standby Mode 0000  0000 (00h)
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

// ADS1256 Input Gain
typedef enum
{
    ADS1256_GAIN_1 = 0, ADS1256_GAIN_2, ADS1256_GAIN_4, ADS1256_GAIN_8, 
        ADS1256_GAIN_16, ADS1256_GAIN_32, ADS1256_GAIN_64
}ADS1256_GAIN_E;

// ADS1256 Sampling Rate
typedef enum
{
    ADS1256_30000SPS = 0, ADS1256_15000SPS, ADS1256_7500SPS, ADS1256_3750SPS,
    ADS1256_2000SPS, ADS1256_1000SPS, ADS1256_500SPS, ADS1256_100SPS, ADS1256_60SPS,
    ADS1256_50SPS, ADS1256_30SPS, ADS1256_25SPS, ADS1256_15SPS, ADS1256_10SPS, ADS1256_5SPS,
    ADS1256_2d5SPS, ADS1256_DRATE_MAX
}ADS1256_DRATE_E;

static const uint8_t s_tabDataRate[ADS1256_DRATE_MAX] =
{
    0xF0,0xE0,0xD0,0xC0, 0xB0,0xA1,0x92,0x82,0x72,
            0x63,0x53,0x43,0x33,0x20,0x13, 0x03
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
unsigned int DAC8532_VoltToValue( double volt , double volt_ref);

// Get AD
int32_t ADS1256_GetAdc(uint8_t _ch);
int32_t ADS1256_GetAdcDiff(uint8_t positive_no , uint8_t negative_no );
double ADS1256_ValueToVolt(int32_t value , double vref);

// Print AD
void ADS1256_PrintAllValue();
void ADS1256_PrintAllValueDiff();
void ADS1256_PrintAllReg();

// AD settings
double ADS1256_SetSampleRate(double rate);
int ADS1256_SetGain(int gain);
uint8_t ADS1256_ReadChipID(void);
static void ADS1256_WriteReg(uint8_t _RegID, uint8_t _RegValue);
static uint8_t ADS1256_ReadReg(uint8_t _RegID);
static void ADS1256_WriteCmd(uint8_t _cmd);

// Privates
void DAC8532_SetCS(char b);
static int32_t ADS1256_ReadData(void);
static void ADS1256_DelayDATA(void);
void ADS1256_WaitDRDY(void);
void ADS1256_ChangeMUX(int8_t positive_no , int8_t negative_no );
void ADS1256_SetCS(char b);
static void ADS1256_Send8Bit(uint8_t _data);
static uint8_t ADS1256_Recive8Bit(void);

