#include "Arduino.h"
#include "BluetoothA2DPSink.h"
#include "AudioTools.h"
#include <Preferences.h>
#include <BLEDevice.h>                   // BLE Stuff
#include <BLEServer.h>                   // BLE Stuff
#include <BLEUtils.h>                    // BLE Stuff
#include <BLE2902.h>                     // ESP32 BLE

#define PREFERENCES "connections"
#define MODE_PIN     34  
#define NAME        "BluetoothSplitter"
#define LED_PIN      LED_BUILTIN
#define BCK_PIN_OUT  26 
#define WS_PIN_OUT   25 
#define DATA_PIN_OUT 33
#define DEFUALT_SAMPLE_RATE 44100

#define SERVICE_UUID           "0000180d-0000-1000-8000-00805f9b34fb"
#define CHARACTERISTIC_UUID_RX "00002a37-0000-1000-8000-00805f9b34fb"
#define CHARACTERISTIC_UUID_TX "fb005c81-02e7-f387-1cad-8acd2d8df0c8"

BluetoothA2DPSink a2dp_sink;
I2SStream i2sOut;

BLECharacteristic *pTxCharacteristic;
BLEServer         *pServer           = NULL;
bool              deviceConnected    = false;
bool              oldDeviceConnected = false;
uint8_t           txValue            = 0;
 
Preferences preferences;
bool config_mode     = false;
bool connections_set = false;
bool reboot          = false;

// Write data to I2S
void on_data_requested(const uint8_t *data, uint32_t length) {
  i2sOut.write(data, length);
}

void setup_connections(){
  connections_set = true;
  String connection0 = preferences.getString("0");
  String connection1 = preferences.getString("1");

  if (connection0.length() > 1){
    String command = String(0) + " start " + connection0;
    Serial.println(command);
    Serial2.print(command);
  }
  delay(500);
  if (connection1.length() > 1){
    String command = String(1) + " start " + connection1;
    Serial.println(command);
    Serial2.print(command);
  }
}

void start_splitter(){
  // Start Bluetooth Audio Receiver
  a2dp_sink.set_stream_reader(on_data_requested, false);
  a2dp_sink.set_auto_reconnect(false);
  a2dp_sink.start(NAME);
  
  auto config = i2sOut.defaultConfig();
    config.port_no         = 0;
    config.pin_bck         = BCK_PIN_OUT;
    config.pin_ws          = WS_PIN_OUT;
    config.pin_data        = DATA_PIN_OUT;
    config.sample_rate     = DEFUALT_SAMPLE_RATE;
    config.channels        = 2;
    config.bits_per_sample = 16;
    config.buffer_count    = 8;
    config.buffer_size     = 256;
  i2sOut.begin(config);
}


void saveConnection(String command){
  if (command.length()>3){
    String id = command.substring(0,1);
    if (id.equals("0") || id.equals("1")){
      String speakername = command.substring(2,command.length());
      preferences.putString(String(id).c_str(), speakername);
      Serial.println("Saving connection " + String(id) + ": " + speakername);
    }else{
       Serial.println("Couldn't recoginze commmand...");
    }
  }else{
    Serial.println("Couldn't recoginze commmand...");
  }
}
void sendConnectMessages(){
  delay(1000);
  String connection0 = preferences.getString("0");
  String connection1 = preferences.getString("1");
  String message0 = "{\"ssid\": \"" + String(connection0) + "\", \"connection\": \"" + String(0) + "\"}";
  String message1 = "{\"ssid\": \"" + String(connection1) + "\", \"connection\": \"" + String(1) + "\"}";
  char msg0[message0.length() + 1] = {}; strcpy(msg0, message0.c_str());
  char msg1[message1.length() + 1] = {}; strcpy(msg1, message1.c_str());
  pTxCharacteristic->setValue((uint8_t*)&msg0, message0.length());pTxCharacteristic->notify();
  Serial.println(msg0);
  delay(1000);
  Serial.println(msg1);
  pTxCharacteristic->setValue((uint8_t*)&msg1, message1.length());pTxCharacteristic->notify();
}
class MyServerCallbacks: public BLEServerCallbacks {
    void onConnect(BLEServer* pServer) {
      deviceConnected = true;
    };
    void onDisconnect(BLEServer* pServer) {
      deviceConnected = false;
    }
};
class MyCallbacks: public BLECharacteristicCallbacks {
    void onWrite(BLECharacteristic *pCharacteristic) {
      std::string rxValue = pCharacteristic->getValue();
      if (rxValue.length() > 0) {
        Serial.println("*********");
        Serial.print("Received Value: ");
        for (int i = 0; i < rxValue.length(); i++)
          Serial.print(rxValue[i]);
        Serial.println();
        Serial.println("*********");
        saveConnection(String(rxValue.c_str()));
      }
    }
};

void start_config_server(){
  delay(500);
  String command = "0 scan";
  Serial2.print(command);

  // Create the BLE Device
  BLEDevice::init(NAME);

  // Create the BLE Server
  pServer = BLEDevice::createServer();
  pServer->setCallbacks(new MyServerCallbacks());

  // Create the BLE Service
  BLEService *pService = pServer->createService(SERVICE_UUID);

  // Create a BLE Characteristic
  pTxCharacteristic = pService->createCharacteristic(
										CHARACTERISTIC_UUID_TX,
										BLECharacteristic::PROPERTY_NOTIFY
									);
                      
  pTxCharacteristic->addDescriptor(new BLE2902());
  BLECharacteristic * pRxCharacteristic = pService->createCharacteristic(
											 CHARACTERISTIC_UUID_RX,
											BLECharacteristic::PROPERTY_WRITE
										);
  pRxCharacteristic->setCallbacks(new MyCallbacks());

  // Start the service
  pService->start();

  // Start advertising
  pServer->getAdvertising()->start();
  return;
}

void change_mode(){
   Serial.println("Changing modes!");
   String command1 = "0 stop";
   String command2 = "1 stop";
   Serial2.print(command1);
   delay(500);
   Serial2.print(command2);
   for (int i=0;i<5;i++){
      delay(250);
      digitalWrite(LED_PIN, !digitalRead(LED_PIN));
   }
   ESP.restart();
}

void handle_commands(){
    if (Serial.available()>0) {
        int len = Serial.available();
        uint8_t buffer[len];
        Serial.readBytes(buffer, Serial.available());
        pTxCharacteristic->setValue((uint8_t*)&buffer, len);
        pTxCharacteristic->notify();
	 }
   if (Serial2.available()>0) {
        int len = Serial2.available();
        uint8_t buffer[len];
        Serial2.readBytes(buffer, len);
        Serial.print("Received speaker name: ");
        Serial.println((char*)buffer);
        pTxCharacteristic->setValue((uint8_t*)&buffer, len);
        pTxCharacteristic->notify();
	 }
    // disconnecting
    if (!deviceConnected && oldDeviceConnected) {
        delay(500); // give the bluetooth stack the chance to get things ready
        pServer->startAdvertising(); // restart advertising
        Serial.println("start advertising");
        oldDeviceConnected = deviceConnected;
        digitalWrite(LED_PIN, LOW);
    }
    // connecting
    if (deviceConnected && !oldDeviceConnected) {
		// do stuff here on connecting
        sendConnectMessages();
        oldDeviceConnected = deviceConnected;
        digitalWrite(LED_PIN, HIGH);
    }
}

void handle_a2dp_state(){
  if (a2dp_sink.is_connected()){
    digitalWrite(LED_PIN,HIGH);
    if (!connections_set){
      setup_connections();
    }
  }else{
    digitalWrite(LED_PIN,LOW);
  }
}

void IRAM_ATTR setReboot() {
    reboot = true;
}

void setup() {
  Serial.begin(115200);
  Serial2.begin(115200);

  pinMode(LED_PIN, OUTPUT);
  digitalWrite(LED_PIN, LOW);
  config_mode = digitalRead(MODE_PIN);
  attachInterrupt(digitalPinToInterrupt(MODE_PIN), setReboot, CHANGE);
  
  //config_mode =  EEPROM.read(MODE);
  preferences.begin(PREFERENCES, false); 

  if (config_mode){
    Serial.println("Starting up config server...");
    start_config_server();
  }else{
    Serial.println("Starting up a2dp splitter...");
    start_splitter();
  }
}

void loop() {
  if (reboot){change_mode();}
  if(config_mode){
    handle_commands();
  }else{
    handle_a2dp_state();
  }
}