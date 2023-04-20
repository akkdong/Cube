// bsp.cpp
//

#include <Arduino.h>
#include <M5EPD.h>

#define UART_DEVICE_RX        (18)
#define UART_DEVICE_TX        (19)



////////////////////////////////////////////////////////////////////////
//

void bsp_hal_init()
{
  pinMode(M5EPD_MAIN_PWR_PIN, OUTPUT);
  M5.enableMainPower();

  Serial.begin(115200);
	Serial1.begin(115200, SERIAL_8N1, UART_DEVICE_RX, UART_DEVICE_TX);
  Wire.begin(21, 22, (uint32_t) 400000U);
  SPI.begin(14, 13, 12, 4);
  delay(20);
  Serial.println("M3 Display module");

  pinMode(M5EPD_EXT_PWR_EN_PIN, OUTPUT);
  pinMode(M5EPD_EPD_PWR_EN_PIN, OUTPUT);
  pinMode(M5EPD_KEY_RIGHT_PIN, INPUT);
  pinMode(M5EPD_KEY_PUSH_PIN, INPUT);
  pinMode(M5EPD_KEY_LEFT_PIN, INPUT);
  delay(20);

  M5.enableEXTPower();  
  M5.enableEPDPower();
  delay(1000);

  M5.EPD.begin(M5EPD_SCK_PIN, M5EPD_MOSI_PIN, M5EPD_MISO_PIN, M5EPD_CS_PIN, M5EPD_BUSY_PIN);
  M5.EPD.Clear(true);
  M5.EPD.SetRotation(M5EPD_Driver::ROTATE_0);
  M5.TP.SetRotation(GT911::ROTATE_0);
  M5.TP.begin(21, 22, 36);
  M5.RTC.begin();
  M5.BatteryADCBegin();

  SD.begin(4, SPI, 20000000);
}