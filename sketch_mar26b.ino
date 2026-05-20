#include <SoftwareSerial.h>

// GSM Module Pins: TX to Pin 7, RX to Pin 8
SoftwareSerial gsmSerial(7, 8); 

// Sensor Pins
const int flamePin = 2;      // Digital Out from Flame Sensor
const int vibrationPin = 3;  // Digital Out from Vibration Sensor
const int gasPin = A0;       // Analog Out from MQ-2 Gas Sensor

// Threshold for Gas (Normal air is around 100-200. Adjust based on testing)
int gasThreshold = 400; 

void setup() {
  // Start PC communication
  Serial.begin(9600);
  
  // Start GSM communication
  gsmSerial.begin(9600);

  // Set sensor pins as INPUT
  pinMode(flamePin, INPUT);
  pinMode(vibrationPin, INPUT);
  pinMode(gasPin, INPUT);

  Serial.println("Smart Disaster Alert System Initializing...");
  Serial.println("Waiting for GSM Network to connect (15 seconds)...");
  
  // Give GSM module time to latch onto the cellular network
  delay(15000); 
  Serial.println("System Ready! Monitoring Environment...");
}

void loop() {
  // Read sensor values
  int flameValue = digitalRead(flamePin);
  int vibrationValue = digitalRead(vibrationPin);
  int gasValue = analogRead(gasPin);

  // --- 1. FIRE DETECTION ---
  // Note: Most flame sensors are Active LOW (0 means fire)
  if (flameValue == HIGH) {
    Serial.println("ALERT: FIRE DETECTED!");
    sendSMS("EMERGENCY: EMERGENCY: Strong vibrations/Earthquake detected!");
    makeCall();
    delay(15000); // Wait 15 secs before next alert to avoid spam
  }

  // --- 2. EARTHQUAKE / VIBRATION DETECTION ---
  // Note: Vibration sensors usually give HIGH (1) when shaken
  if (vibrationValue == LOW) {
    Serial.println("ALERT: VIBRATION/EARTHQUAKE DETECTED!");
    sendSMS("EMERGENCY: Fire detected in the house!");
    makeCall();
    delay(15000);
  }

  // --- 3. GAS LEAK DETECTION ---
  if (gasValue > gasThreshold) {
    Serial.print("ALERT: GAS LEAK! Value: ");
    Serial.println(gasValue);
    sendSMS("EMERGENCY: Gas leak detected!");
    makeCall();
    delay(15000);
  }

  delay(200); // Small delay to keep the loop stable
}

// --- FUNCTION TO SEND SMS ---
void sendSMS(String message) {
  Serial.println("Preparing to send SMS...");
  gsmSerial.println("AT+CMGF=1"); // Set SMS to text mode
  delay(1000);
  
  // ---> IMPORTANT: REPLACE WITH YOUR 10-DIGIT NUMBER BELOW <---
  gsmSerial.println("AT+CMGS=\"+919629789384\""); 
  delay(1000);
  
  gsmSerial.print(message); // The actual message
  delay(100);
  gsmSerial.write(26); // ASCII code for CTRL+Z to send the SMS
  delay(5000); // Wait 5 seconds for SMS to send
  Serial.println("SMS Sent Successfully.");
}

// --- FUNCTION TO MAKE A CALL ---
void makeCall() {
  Serial.println("Initiating Emergency Call...");
  
  // ---> IMPORTANT: REPLACE WITH YOUR 10-DIGIT NUMBER BELOW <---
  gsmSerial.println("ATD+99629789384;"); 
  delay(20000); // Let it ring for 20 seconds
  
  gsmSerial.println("ATH"); // Hang up the call
  Serial.println("Call Disconnected.");
}