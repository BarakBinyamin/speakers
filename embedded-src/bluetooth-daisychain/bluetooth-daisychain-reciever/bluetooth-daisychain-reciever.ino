/*
  bluetooth-daisychain-reciever
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
const size_t max_buffer_len = 150;
const int channels = 2;
const size_t max_buffer_bytes = max_buffer_len * sizeof(int16_t) * channels;
uint8_t buffer[max_buffer_bytes]={0};

// These buffers are used to delay music going the i2s speaker so that the next bluetooth speaker sync up
const size_t num_buffers = 50; // Change based on distance, and number in chain
int32_t delayBufferReadNum[num_buffers]           = {max_buffer_bytes};
uint8_t delayBuffer[num_buffers*max_buffer_bytes] = {0};
int buffer_num                                    = 0;

// Callback used by A2DP to provide the sound data
int32_t on_data_requested (uint8_t *data, int32_t len) {
  size_t req_bytes     = min(max_buffer_bytes, len*sizeof(uint8_t));
  size_t result_bytes  = i2sIn.readBytes((uint8_t*)data, req_bytes);
  
  uint8_t *save_buffer = &delayBuffer[ ((buffer_num)*max_buffer_bytes) ];
  uint8_t *play_buffer = &delayBuffer[ (((buffer_num+1)%num_buffers)*max_buffer_bytes) ];
  int32_t num_bytes_to_play = delayBufferReadNum[ (buffer_num+1)%num_buffers ];
  memcpy(save_buffer, data, result_bytes);
  delayBufferReadNum[buffer_num] = result_bytes;

  i2sOut.write(play_buffer,num_bytes_to_play);
  buffer_num = (buffer_num + 1)%num_buffers;

  return result_bytes;
}

// Arduino Setup
void setup(void) {
  // Nice debug stuff
  Serial.begin(115200);
  AudioLogger::instance().begin(Serial, AudioLogger::Info);

  // Start I2S Stuff
  Serial.println("Starting I2S...");
  auto cfg = i2sIn.defaultConfig(RX_MODE);
    cfg.port_no         = 1;
    cfg.i2s_format      = I2S_PHILIPS_FORMAT; // or try with I2S_LSB_FORMAT
    cfg.bits_per_sample = 16;
    cfg.buffer_count    = 8;
    cfg.buffer_size     = 256;
    cfg.channels        = 2;
    cfg.sample_rate     = 44100;
    cfg.pin_bck         = SCK_PIN_IN;
    cfg.pin_ws          = WS_PIN_IN;
    cfg.pin_data        = DATA_PIN_IN;
    cfg.pin_data_rx     = DATA_PIN_IN;
    cfg.is_master       = false;
  i2sIn.begin(cfg);                           // From Other Board

  auto cfg2 = i2sOut.defaultConfig();
    cfg2.port_no         = 0;
    cfg2.pin_bck         = SCK_PIN_OUT;
    cfg2.pin_ws          = WS_PIN_OUT;
    cfg2.pin_data        = DATA_PIN_OUT;
    cfg2.sample_rate     = 44100;            // Reconsider this, maybe get sample rate from i2sIn
    cfg2.channels        = 2;
    cfg2.bits_per_sample = 16;
    cfg2.buffer_count    = 8;
    cfg2.buffer_size     = 256;
  i2sOut.begin(cfg2);                        // To Speaker

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
    size_t req_bytes    = max_buffer_bytes;
    size_t result_bytes = i2sIn.readBytes(delayBuffer, req_bytes);
    i2sOut.write(delayBuffer, result_bytes);
  }
}