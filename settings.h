//Connections:
// pin 7 - Serial clock out (SCLK)
// pin 6 - Serial data out (DIN)
// pin 5 - Data/Command select (D/C)
// pin 4 - LCD chip select (CS)
// pin 3 - LCD reset (RST)

//#define height 48
//#define width 84

//#define LIN_OUT 1 // включаем функцию для линейного формата
#define LOG_OUT 1 // включаем функцию для линейного формата
#define FFT_N 256 // задаем количество выходных отсчетов на 256

const int CS_MCP3008      = 53;          // ADC Chip Select
const byte adc_single_ch0  = (0x08);     // ADC Channel 0   signal
const byte adc_single_ch1  = (0x09);     // ADC Channel 1
const byte adc_single_ch2  = (0x0A);     // ADC Channel 2
const byte adc_single_ch3  = (0x0B);     // ADC Channel 3   Синхронизация
const byte adc_single_ch4  = (0x0C);     // ADC Channel 4
const byte adc_single_ch5  = (0x0D);     // ADC Channel 5   Potenciometr 2
const byte adc_single_ch6  = (0x0E);     // ADC Channel 6
const byte adc_single_ch7  = (0x0F);     // ADC Channel 7   Potenciometr 1

int adc_buffer[1450];                    //задаем размер буффера

int xi = 0;                              //задаем начальное положение курсора гистограммы
double yzoom = 1;                        //задаем начальное увеличение осциллографа
double spzoom = 1; 
byte N = 91;                             //задаем количество точек для построение осциллограммы

int del;
