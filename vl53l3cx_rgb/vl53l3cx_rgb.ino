#include <Arduino.h>
#include <Wire.h>
#include <vl53lx_class.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <assert.h>
#include <stdlib.h>
#include <Adafruit_NeoPixel.h>

// Define I2C, Serial, and LED pins
#define DEV_I2C Wire
#define SerialPort Serial

// NeoPixel LED on pin 38
Adafruit_NeoPixel LED_RGB(1, 38, NEO_GRBW + NEO_KHZ800);

// Define LED colors for each object count
struct Color {
  uint8_t r;
  uint8_t g;
  uint8_t b;
  uint8_t w;
};

const Color colors[] = {
  {255, 255, 255, 255}, // 0 Objects: WHITE
  {255, 0, 0, 0},       // 1 Object: RED
  {255, 255, 0, 0},     // 2 Objects: YELLOW
  {0, 255, 0, 0},       // 3 Objects: GREEN
  {0, 0, 255, 0}        // 4 Objects: BLUE
};

// Components.
VL53LX sensor_vl53lx_sat(&DEV_I2C, A1);

/* Setup ---------------------------------------------------------------------*/
void setup()
{
   // Initialize NeoPixel
   LED_RGB.begin();
   LED_RGB.setBrightness(150);

   // Initialize serial for output.
   SerialPort.begin(115200);
   SerialPort.println("Starting...");

   // Initialize I2C bus.
   DEV_I2C.begin(8, 9);
   SerialPort.println("I2C started");

   // Configure VL53LX satellite component.
   sensor_vl53lx_sat.begin();
   SerialPort.println("Sensor started");

   // Switch off VL53LX satellite component.
   sensor_vl53lx_sat.VL53LX_Off();
   SerialPort.println("Switch off sensor sat component");

   // Initialize VL53LX satellite component.
   sensor_vl53lx_sat.InitSensor(0x12);
   SerialPort.println("Init sensor started");

   // Start Measurements
   sensor_vl53lx_sat.VL53LX_StartMeasurement();
   SerialPort.println("Start measuring");
}

void loop()
{
   VL53LX_MultiRangingData_t MultiRangingData;
   VL53LX_MultiRangingData_t *pMultiRangingData = &MultiRangingData;
   uint8_t NewDataReady = 0;
   int no_of_object_found = 0, j;
   char report[64];
   int status;

   // Wait for new measurement data to be ready
   do
   {
      status = sensor_vl53lx_sat.VL53LX_GetMeasurementDataReady(&NewDataReady);
   } while (!NewDataReady);

   if((!status) && (NewDataReady != 0))
   {
      // Get ranging data and print serial output as in the original code
      status = sensor_vl53lx_sat.VL53LX_GetMultiRangingData(pMultiRangingData);
      no_of_object_found = pMultiRangingData->NumberOfObjectsFound;
      snprintf(report, sizeof(report), "VL53LX Satellite: Count=%d, #Objs=%1d ", pMultiRangingData->StreamCount, no_of_object_found);
      SerialPort.print(report);
      
      for(j = 0; j < no_of_object_found; j++)
      {
         if(j != 0) SerialPort.print("\r\n                               ");
         SerialPort.print("status=");
         SerialPort.print(pMultiRangingData->RangeData[j].RangeStatus);
         SerialPort.print(", D=");
         SerialPort.print(pMultiRangingData->RangeData[j].RangeMilliMeter);
         SerialPort.print("mm");
         SerialPort.print(", Signal=");
         SerialPort.print((float)pMultiRangingData->RangeData[j].SignalRateRtnMegaCps / 65536.0);
         SerialPort.print(" Mcps, Ambient=");
         SerialPort.print((float)pMultiRangingData->RangeData[j].AmbientRateRtnMegaCps / 65536.0);
         SerialPort.print(" Mcps");
      }
      SerialPort.println("");

      // Set LED color based on object count (capped at 4)
      int colorIndex = min(no_of_object_found, 4);
      LED_RGB.setPixelColor(0, LED_RGB.Color(colors[colorIndex].r, colors[colorIndex].g, colors[colorIndex].b, colors[colorIndex].w));
      LED_RGB.show();

      // Clear interrupt and start new measurement
      if (status == 0) {
         status = sensor_vl53lx_sat.VL53LX_ClearInterruptAndStartMeasurement();
      }
   }

  //delay(1000); // Wait a moment before checking again
}
