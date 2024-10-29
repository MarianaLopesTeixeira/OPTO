#include <Arduino.h>
#include <Wire.h>
#include <vl53lx_class.h>
#include <Adafruit_NeoPixel.h>

#define DEV_I2C Wire
#define SerialPort Serial
#define RX_PIN 18
#define TX_PIN 17

Adafruit_NeoPixel LED_RGB(1, 38, NEO_GRBW + NEO_KHZ800);

struct Color {
  uint8_t r;
  uint8_t g;
  uint8_t b;
  uint8_t w;
};

const Color colors[] = {
  { 255, 255, 255, 255 },  // 0 Objects: WHITE
  { 255, 0, 0, 0 },        // 1 Object: RED
  { 255, 255, 0, 0 },      // 2 Objects: YELLOW
  { 0, 255, 0, 0 },        // 3 Objects: GREEN
  { 0, 0, 255, 0 }         // 4 Objects: BLUE
};

VL53LX sensor_vl53lx_sat(&DEV_I2C, A1);

void setup() {
  LED_RGB.begin();
  LED_RGB.setBrightness(150);
  SerialPort.begin(115200);
  SerialPort.println("Starting...");
  DEV_I2C.begin(8, 9);
  SerialPort.println("I2C started");

  sensor_vl53lx_sat.begin();
  sensor_vl53lx_sat.VL53LX_Off();
  sensor_vl53lx_sat.InitSensor(0x12);
  sensor_vl53lx_sat.VL53LX_StartMeasurement();
  SerialPort.println("Sensor started");

  pinMode(TX_PIN, OUTPUT);
  Serial1.begin(9600, SERIAL_8N1, RX_PIN, TX_PIN);
  SerialPort.println("Laser ON");
}

void loop() {
  VL53LX_MultiRangingData_t MultiRangingData;
  VL53LX_MultiRangingData_t *pMultiRangingData = &MultiRangingData;
  uint8_t NewDataReady = 0;
  int no_of_object_found = 0, j;
  char report[128];
  int status;

  do {
    status = sensor_vl53lx_sat.VL53LX_GetMeasurementDataReady(&NewDataReady);
  } while (!NewDataReady);

  if ((!status) && (NewDataReady != 0)) {
    status = sensor_vl53lx_sat.VL53LX_GetMultiRangingData(pMultiRangingData);
    no_of_object_found = pMultiRangingData->NumberOfObjectsFound;

    String dataString = "";
    snprintf(report, sizeof(report), "VL53LX Satellite: Count=%d, #Objs=%1d ",
             pMultiRangingData->StreamCount, no_of_object_found);
    dataString += report;

    for (j = 0; j < no_of_object_found; j++) {
      snprintf(report, sizeof(report), "Status=%d, D=%dmm, Signal=%.2fMcps, Ambient=%.2fMcps ",
               pMultiRangingData->RangeData[j].RangeStatus,
               pMultiRangingData->RangeData[j].RangeMilliMeter,
               (float)pMultiRangingData->RangeData[j].SignalRateRtnMegaCps / 65536.0,
               (float)pMultiRangingData->RangeData[j].AmbientRateRtnMegaCps / 65536.0);
      dataString += report;
    }

    SerialPort.println();  // Only one newline for the received data, no extra line
    SerialPort.println();  // Only one newline for the received data, no extra line

    // Print sensor data to Serial
    SerialPort.println("Sensor Data:      " + dataString);

    // Set LED color
    int colorIndex = min(no_of_object_found, 4);
    LED_RGB.setPixelColor(0, LED_RGB.Color(colors[colorIndex].r, colors[colorIndex].g, colors[colorIndex].b, colors[colorIndex].w));
    LED_RGB.show();

    // Clear Serial1 buffer before sending new data
    while (Serial1.available()) {
      Serial1.read();
    }

    // Transmit data with delimiter
    //dataString += "\n";  // Add newline as delimiter
    SerialPort.println("Sending Data TX:  " + dataString);
    Serial1.print(dataString);
    Serial1.flush();  // Wait for data to be sent

    if (status == 0) {
      status = sensor_vl53lx_sat.VL53LX_ClearInterruptAndStartMeasurement();
    }
  }

  // Receive data with delimiter handling
  String receivedDataString = "";
  while (Serial1.available()) {
    char c = Serial1.read();
    if (c == '\n') {
      break;  // End of message detected
    }
    receivedDataString += c;
  }

  // Print received data to Serial
  SerialPort.print("Received Data RX: ");
  if (receivedDataString.length() > 0) {
    SerialPort.print(receivedDataString);
  }

  delay(1000);  // Short delay for stability
}
