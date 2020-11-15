#include "DHT.h"
//#include <SoftwareSerial.h>
//#include "AltSoftSerial.h"
#include <ArduinoBLE.h>

#define DHTPIN 1     // what digital pin we're connected to

#define DHTTYPE DHT11   // DHT 11
//#define DHTTYPE DHT22   // DHT 22  (AM2302), AM2321
//#define DHTTYPE DHT21   // DHT 21 (AM2301)

DHT dht(DHTPIN, DHTTYPE);
//AltSoftSerial ble_device(0,1); // HM-10 TX/RX pins

// BLE Temp Service
BLEService TempService("180F"); 

// BLE Temp Characteristic
BLEUnsignedCharCharacteristic TempChar("2A19", BLERead | BLENotify); 

float Temp = 0; //last Temp reading, initial 

void setup() { 
  dht.begin(); // prepare DHT sensor
  Serial.begin(9600); 
  while (!Serial);

  pinMode(LED_BUILTIN, OUTPUT);

  // begin initialization
  if (!BLE.begin()) {
    Serial.println("starting BLE failed!");

    while (1);
  }
  
  /* Set a local name for the BLE device
     This name will appear in advertising packets
     and can be used by remote devices to identify this BLE device
     The name can be changed but maybe be truncated based on space left in advertisement packet
  */
  BLE.setLocalName("TempDataLogger");
  BLE.setAdvertisedService(TempService); // add the service UUID
  TempService.addCharacteristic(TempChar); // add the battery level characteristic
  BLE.addService(TempService); // Add the battery service
  TempChar.writeValue(Temp); // set initial value for this characteristic

  /* Start advertising BLE.  It will start continuously transmitting BLE
     advertising packets and will be visible to remote BLE central devices
     until it receives a new connection */

  // start advertising
  BLE.advertise();

  Serial.println("Bluetooth device active, waiting for connections...");
  //ble_device.begin(9600); // prepare BLE module
}

void loop() {
  // wait for a BLE central
  BLEDevice central = BLE.central();

  // if a central is connected to the peripheral:
  if (central) {
    Serial.print("Connected to central: ");
    // print the central's BT address:
    Serial.println(central.address());
    // turn on the LED to indicate the connection:
    digitalWrite(LED_BUILTIN, HIGH);

  while (central.connected()) {
    delay(2000); // delay in-between readings
    //float f = dht.readHumidity(); // uncomment to send humidity
    float f = dht.readTemperature(true); // send temperature data
    //char f_str[6]; // prepare character array to send
    //dtostrf(f,2,1,f_str); // format data into char array
    //ble_device.write(f_str); // send to BLExAR
    Serial.print("Temp is now: ");
    Serial.println(f); 
    TempChar.writeValue(f);
  }
 }     
}
