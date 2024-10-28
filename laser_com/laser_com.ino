// Define the pin for the laser
#define RX_PIN 18
#define TX_PIN 17

// Define the binary data to be sent (for example, a sequence of "1"s and "0"s)
//const String binaryData = "1111111111111111";  // Example binary data
const String binaryData = "0000000000000000";  // Example binary data
int errors = 0;    // Count of errors
int totalBits = 0; // Total bits sent

void setup() {
  // Initialize Serial communication for debugging
  Serial.begin(9600);
  Serial.println("Starting...");

  // Set the laser pin as an output
  pinMode(TX_PIN, OUTPUT);

  // Initialize Serial1 for laser communication
  Serial1.begin(9600, SERIAL_8N1, RX_PIN, TX_PIN);
  Serial.println("Laser ON");
}

void loop() {
  // Sending binary data
  for (char bit : binaryData) {
    Serial1.write(bit); // Send each bit
    Serial.print("SENDING TX ");
    Serial.println(bit);
    totalBits++; // Increment total bits sent
    delay(10); // Delay for stability in transmission
  }

  // Check for received data
  while (Serial1.available()) {
    char receivedBit = Serial1.read();
    Serial.print("READING RX ");
    Serial.println(receivedBit);
    
    // Compare received data with sent data
    if (receivedBit != binaryData[totalBits - totalBits]) {
      errors++; // Increment error count if there is a mismatch
    }
  }

  // Calculate and print error rate
  if (totalBits > 0) {
    float errorRate = (float)errors / totalBits * 100; // Calculate error rate in percentage
    Serial.print("Total Bits Sent: ");
    Serial.println(totalBits);
    Serial.print("Errors Detected: ");
    Serial.println(errors);
    Serial.print("Error Rate: ");
    Serial.print(errorRate);
    Serial.println("%");
  }

  // Delay before sending the data again
  delay(1000);
}
