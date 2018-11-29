//Arduino 1.6.0 required//

#include <SPI.h>                                                        // Include the SPI library
#include "settings.h"
#include <Adafruit_GFX.h>                                               // Include graphics library
#include <Adafruit_PCD8544.h>                                           // Include display library
#include "FFT.h"                                                        // Include FFT library

Adafruit_PCD8544 display = Adafruit_PCD8544(7, 6, 5, 4, 3);             //Initializing display
SPISettings MCP3008(2000000, MSBFIRST, SPI_MODE0);                      //Initializing ADC

volatile byte GistoOn = 0;                                              //Display mode(Osci/Gisto)
char dat[83];
int Fourier[40];


void setup()   {
  SPI.begin     ();
  Serial.begin  (2000000);
  pinMode       (CS_MCP3008, OUTPUT);
  digitalWrite  (CS_MCP3008, LOW);        // Latch
  digitalWrite  (CS_MCP3008, HIGH);
  display.begin();
  
  //display.setContrast(100);
  //display.display();
  //delay(50);
  display.clearDisplay();

  display.setTextColor(BLACK);
  display.setTextSize(1);
  display.drawRect(0, 0, 84,  48,  BLACK);  
  display.setCursor(5, 3);
  display.println("Harmonic       analyser of     voltage");
  display.display();
  delay(2000);  
  display.clearDisplay();
  display.drawRect(0, 0, 84,  48,  BLACK);
  display.setCursor(30, 13);
  display.println("NSTU");
  display.setCursor(3, 3);
  display.println("Kafedra TEVN");
  display.display();
  delay(2000);
  display.clearDisplay();
  attachInterrupt(4, OsciScreen, RISING);
  attachInterrupt(5, Gisto, RISING);
}

void drawAxis(){
  display.drawRect(0, 0, 84,  48,  BLACK);
  display.drawFastHLine(0, 48/2, 84, BLACK);
  for (byte i=0; i<48/2; i++){
    display.drawFastHLine(0, 48-2*i, 2, BLACK);
  }
    for (byte i=0; i<43; i++){
    display.drawFastVLine(2*i,48/2-1, 3, BLACK);
  }
  display.display();
}
void drawAxisGisto(){
  display.drawRect(0, 0, 84,  48,  BLACK);
  display.drawRect(25, 0, 84, 11,  BLACK);
  display.display();
  for (byte i=0; i<48/2; i++){
    display.drawFastHLine(0, 48-2*i, 2, BLACK);
  }
  display.display();
  display.setCursor(26, 2);
  display.println(xi);
  display.drawFastVLine(37, 0, 11, BLACK); 
  display.setCursor(44, 2);
  display.println(Fourier[xi-1]*100/10);                 
  display.setCursor(70, 2);
  display.println("mV");
  display.display();  
}

void OsciScreen(){ 
      GistoOn = 0; 
      display.clearDisplay();
      drawAxis();
                                                        display.setCursor(2,2);
                                                        display.println(del); 
      display.setCursor(59,2);
      display.println(yzoom);                 
      display.display();
      int spd = adc_single_channel_read (adc_single_ch3);
      for (byte i=0;i<N;i++){
          display.drawLine(i+1, 48/2-dat[i], i+2, 48/2-dat[i+1], BLACK);
          delayMicroseconds(spd);
      } 
      display.display();
} 

void SpectrScreen(){
      display.clearDisplay();
      for (int i = 0; i < 512; i++) {
      fft_input[i] = adc_buffer[i];
      fft_input[i+1] = 0;
      }
      //fft_window();
      fft_reorder(); // реорганизовываем данные перед запуском БПФ
      fft_run(); // обрабатываем данные в БПФ
      //fft_mag_lin(); // извлекаем данные, обработанные БПФ
      fft_mag_log(); // извлекаем данные, обработанные БПФ
      //fft_lin_out[0] = 0;
      fft_log_out[0] = 0;
      drawAxisGisto(); 
      display.drawLine(2*xi+1, 11, 2*xi+1, 14, BLACK);
      display.drawPixel(2*xi, 13, BLACK);
      display.drawPixel(2*xi+2, 13, BLACK);
      display.display();
      for (byte i=0;i<40;i++){
           display.drawLine(2*i+3, 48 - Fourier[i]*spzoom/10, 2*i+3, 48, BLACK);           
      }  
      display.display();
      delay(200);
      //delay(1800);
}

void Gisto(){
  GistoOn = 1;
}
int adc_single_channel_read(byte readAddress){

  byte dataMSB =    0;
  byte dataLSB =    0;
  byte JUNK    = 0x00;

  SPI.beginTransaction (MCP3008);
  digitalWrite         (CS_MCP3008, LOW);
  SPI.transfer         (0x01);                                 // Start Bit
  dataMSB =            SPI.transfer(readAddress << 4) & 0x03;  // Send readAddress and receive MSB data, masked to two bits
  dataLSB =            SPI.transfer(JUNK);                     // Push junk data and get LSB byte return
  digitalWrite         (CS_MCP3008, HIGH);
  SPI.endTransaction   ();

  return               dataMSB << 8 | dataLSB;

}

void loop() {
  del = adc_single_channel_read (adc_single_ch7);
  int z = adc_single_channel_read (adc_single_ch5);
    for (int i = 0; i <= 1245; i++) {
    adc_buffer[i] = adc_single_channel_read (adc_single_ch0);
    delayMicroseconds(10);                                           //Задержка для нормального ДПФ
  }
  
  if (GistoOn ==1){
    xi = adc_single_channel_read (adc_single_ch7);
    xi = map(xi,0,1023,1,40);
    spzoom = adc_single_channel_read (adc_single_ch5);
    spzoom = map(spzoom,0,1023,1,20);
    SpectrScreen();

      for (byte i=0;i<40;i++){
        Fourier[i] = fft_log_out[i+1];
        //if (Fourier[i]<10){                                         //Фильтр от помех при БПФ
         //Fourier[i] = 0; 
        //}
      }
      
  }
  else
  {
  yzoom = adc_single_channel_read (adc_single_ch5);
  yzoom = map(yzoom,0,1023,1,20)/4.0;
  
  for (byte i = 0; i<83; i++){
    dat[i] = yzoom*((adc_buffer[15*i]-500)/22);                         ///Обрезка выборки и масштабирование
  }
  OsciScreen();
  delay(70);
}
}



