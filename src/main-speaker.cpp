#include "Arduino.h"
#include "BluetoothA2DPSink.h"
#include "AudioTools.h"

#define SPEAKER_NAME "Rockz Speaker"
#define LED_PIN      LED_BUILTIN
#define BCK_PIN_OUT  26 
#define WS_PIN_OUT   25 
#define DATA_PIN_OUT 33
#define DEFUALT_SAMPLE_RATE 44100

BluetoothA2DPSink a2dp_sink;
I2SStream i2sOut;

// Write data to I2S
void on_data_requested(const uint8_t *data, uint32_t length) {
  i2sOut.write(data, length);
}

void setup() {
  Serial.begin(115200);
  // AudioLogger::instance().begin(Serial, AudioLogger::Info);  

  // Start Bluetooth Audio Receiver
  a2dp_sink.set_stream_reader(on_data_requested, false);
  a2dp_sink.set_auto_reconnect(true);
  a2dp_sink.start(SPEAKER_NAME);
  
  // Start i2s stuff
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