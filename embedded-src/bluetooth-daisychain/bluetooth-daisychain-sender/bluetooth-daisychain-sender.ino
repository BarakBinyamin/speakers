/*
  bluetooth-daisychain-sender
*/

#include "BluetoothA2DPSink.h"
#include "AudioTools.h"

#define SPEAKER_NAME "DataComDemo"
#define LED_PIN      2
#define SCK_PIN_OUT  26 
#define WS_PIN_OUT   25 
#define DATA_PIN_OUT 33

BluetoothA2DPSink a2dp_sink;
I2SStream i2sOut;

// Write data to I2S
void read_data_stream(const uint8_t *data, uint32_t length) {
  i2sOut.write(data, length);
}

void setup() {
  Serial.begin(115200);
  AudioLogger::instance().begin(Serial, AudioLogger::Info);

  // register callback
  a2dp_sink.set_stream_reader(read_data_stream, false);

  // Start Bluetooth Audio Receiver
  a2dp_sink.set_auto_reconnect(false);
  a2dp_sink.start("DataComDemo");
  
  // Start i2s stuff
  auto cfg = i2sOut.defaultConfig();
    cfg.port_no         = 0;
    cfg.pin_bck         = SCK_PIN_OUT;
    cfg.pin_ws          = WS_PIN_OUT;
    cfg.pin_data        = DATA_PIN_OUT;
    cfg.sample_rate     = a2dp_sink.sample_rate();
    cfg.channels        = 2;
    cfg.bits_per_sample = 16;
    cfg.buffer_count    = 8;
    cfg.buffer_size     = 256;
  i2sOut.begin(cfg);

  pinMode(LED_PIN, OUTPUT);
  digitalWrite(LED_PIN, LOW);
}

void loop() {
  if (a2dp_sink.is_connected()){
    digitalWrite(LED_PIN,HIGH);
  }else{
    digitalWrite(LED_PIN,LOW);
  }
}