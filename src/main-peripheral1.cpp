#include "AudioTools.h"
#include "AudioLibs/AudioA2DP.h"

/*
Recieves Commands over uart
Recieves I2S
Sends    A2DP
*/

#define ID           "1"
#define LED_PIN      BUILTIN_LED
#define BCK_PIN_IN   32      
#define WS_PIN_IN    35      
#define DATA_PIN_IN  34      

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

// Callback used by A2DP to provide the sound data, source with data from i2s
int32_t on_data_requested (uint8_t *data_to_fill, int32_t len) {
  size_t num_bytes_to_request  = min(max_buffered_bytes, len*sizeof(uint8_t));
  size_t num_bytes_read        = i2sIn.readBytes((uint8_t*)data_to_fill, num_bytes_to_request);
  
  // uint8_t *save_buffer = &delayBuffer[ ((buffer_num)*max_buffered_bytes) ];
  // uint8_t *play_buffer = &delayBuffer[ (((buffer_num+1)%num_buffers)*max_buffered_bytes) ];
  // int32_t num_bytes_to_play = delayBufferReadNum[ (buffer_num+1)%num_buffers ];
  // memcpy(save_buffer, data, num_bytes_read);
  // delayBufferReadNum[buffer_num] = num_bytes_read;

  // i2sOut.write(play_buffer,num_bytes_to_play);
  // buffer_num = (buffer_num + 1)%num_buffers;

  return num_bytes_read;
}

void setup(void) {
  Serial.begin(115200);
  Serial2.begin(115200);

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
  i2sIn.begin(config);              

  pinMode(LED_PIN, OUTPUT);
  digitalWrite(LED_PIN, LOW);
}

void start_speaker(const char* speaker_name){
  a2dp_source.set_auto_reconnect(false);
  a2dp_source.start_raw(speaker_name, on_data_requested); 
  a2dp_source.set_volume(100);
}

void stop_speaker(){
  for (int i=0;i<5;i++){
    delay(250);
    digitalWrite(LED_PIN, !digitalRead(LED_PIN));
   }
  ESP.restart();
}

void handle_commands(){
  size_t num_bytes_available = Serial2.available();
  if (num_bytes_available){

    Serial.print("Receieved command: ");
    char command[num_bytes_available];
    Serial2.readBytes(command, num_bytes_available);
    String readable_cmd = String(command);
    Serial.println(readable_cmd);

    String id = readable_cmd.substring(0,1);
    if (!id.equals(ID)){return;}

    if (num_bytes_available>6){
      if (readable_cmd.substring(2,7).equals("start")){
        String speaker_name   = readable_cmd.substring(8,num_bytes_available);
        start_speaker(speaker_name.c_str());
        Serial.print("Attempting to connect to speaker: ");
        Serial.println(speaker_name.c_str());
      }else{
        Serial.println("Receieved unkown command...");
      }
    }else if (num_bytes_available>5){
      if(readable_cmd.substring(2,6).equals("stop")){
        stop_speaker();
        Serial.print("Stopping speaker: ");
      }else{
        Serial.println("Receieved unkown command...");
      }
    }
    
  }
}

void loop() {
  if (a2dp_source.is_connected()){
    digitalWrite(LED_PIN,HIGH);
  }else{
    digitalWrite(LED_PIN,LOW);
  }
  handle_commands();
}