/*
  bluetooth-daisychain-sender
*/

#include "AudioTools.h"
#include "AudioLibs/AudioA2DP.h"

#define CONNECT_TO   "JBL Charge 3"
#define LED_PIN      2
#define BCK_PIN_IN   32      
#define WS_PIN_IN    35      
#define DATA_PIN_IN  34      
#define BCK_PIN_OUT  26 
#define WS_PIN_OUT   25 
#define DATA_PIN_OUT 33 

BluetoothA2DPSource a2dp_source;
I2SStream i2sIn;
I2SStream i2sOut;
const int    channels                   = 2;
const size_t max_buffered_packets       = 150;
const size_t max_buffered_bytes         = max_buffered_packets * sizeof(int16_t) * channels;
uint8_t      buffer[max_buffered_bytes] = {0};

// These buffers are used to delay music going the i2s speaker so that the next bluetooth speaker can sync up
// Change based longest distance, and number in chain, about 50 buffers per 6 feet
const size_t num_buffers                            = 50; 
int32_t delayBufferReadNum[num_buffers]             = {max_buffered_bytes};
uint8_t delayBuffer[num_buffers*max_buffered_bytes] = {0};
int buffer_num                                      = 0;

// Callback used by A2DP to provide the sound data
int32_t on_data_requested (uint8_t *data, int32_t len) {
  size_t num_bytes_to_request  = min(max_buffered_bytes, len*sizeof(uint8_t));
  size_t num_bytes_read        = i2sIn.readBytes((uint8_t*)data, num_bytes_to_request);
  
  uint8_t *save_buffer = &delayBuffer[ ((buffer_num)*max_buffered_bytes) ];
  uint8_t *play_buffer = &delayBuffer[ (((buffer_num+1)%num_buffers)*max_buffered_bytes) ];
  int32_t num_bytes_to_play = delayBufferReadNum[ (buffer_num+1)%num_buffers ];
  memcpy(save_buffer, data, num_bytes_read);
  delayBufferReadNum[buffer_num] = num_bytes_read;

  i2sOut.write(play_buffer,num_bytes_to_play);
  buffer_num = (buffer_num + 1)%num_buffers;

  return num_bytes_read;
}

// Arduino Setup
void setup(void) {
  // Nice debug stuff
  Serial.begin(115200);
  AudioLogger::instance().begin(Serial, AudioLogger::Info);

  // Start I2S Stuff
  Serial.println("Starting I2S...");
  auto config = i2sIn.defaultConfig(RX_MODE);
    config.port_no         = 1;
    config.i2s_format      = I2S_PHILIPS_FORMAT;
    config.bits_per_sample = 16;
    config.buffer_count    = 8;
    config.buffer_size     = 256;
    config.channels        = 2;
    config.sample_rate     = 44100;
    config.pin_bck         = BCK_PIN_IN;
    config.pin_ws          = WS_PIN_IN;
    config.pin_data        = DATA_PIN_IN;
    config.pin_data_rx     = DATA_PIN_IN;
    config.is_master       = false;
  i2sIn.begin(config);                           // From Other Board

  auto config2 = i2sOut.defaultConfig();
    config2.port_no         = 0;
    config2.pin_bck         = BCK_PIN_OUT;
    config2.pin_ws          = WS_PIN_OUT;
    config2.pin_data        = DATA_PIN_OUT;
    config2.sample_rate     = 44100;             // Reconsider this, maybe get sample rate from i2sIn
    config2.channels        = 2;
    config2.bits_per_sample = 16;
    config2.buffer_count    = 8;
    config2.buffer_size     = 256;
  i2sOut.begin(config2);                         // To Speaker

  // Start The Bluetooth 
  Serial.println("Starting bluetooth speaker source...");
  a2dp_source.set_auto_reconnect(true);
  a2dp_source.start_raw(CONNECT_TO, on_data_requested); 
  a2dp_source.set_volume(100);

  pinMode(LED_PIN, OUTPUT);
  digitalWrite(LED_PIN, LOW);
}

void loop() {
  if (a2dp_source.is_connected()){
    digitalWrite(LED_PIN,HIGH);
  }else{
    digitalWrite(LED_PIN,LOW);
    size_t num_bytes_to_request  = max_buffered_bytes;
    size_t num_bytes_read        = i2sIn.readBytes(delayBuffer, num_bytes_to_request);
    i2sOut.write(delayBuffer, num_bytes_read);
  }
}