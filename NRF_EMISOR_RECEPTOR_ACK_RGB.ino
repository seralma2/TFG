#include <nRF24L01.h>
#include <RF24.h>
#include <RF24_config.h>
#include <Adafruit_NeoPixel.h>
#include <SPI.h>
#include <ArduinoJson.h>
#include <ArduinoJson.hpp>
#define  PIN 7
#define radar  4
#define myId 2
#define NUMPIXELS 12

#ifdef __AVR__
 #include <avr/power.h> // Required for 16 MHz Adafruit Trinket
#endif
Adafruit_NeoPixel pixels(NUMPIXELS, PIN, NEO_GRB + NEO_KHZ800);
const int pinCE = 9;
const int pinCSN = 10;
RF24 radio(pinCE, pinCSN);
String mensaje_recibido;
const int LDR = A0;
int val_LDR = /*MAX LDR VALUE*/;
StaticJsonDocument <200> doc;
bool emer;
char data[128];
int vel = 0;
// Single radio pipe address for the 2 nodes to communicate.
const uint64_t pipe [3] = {0xF0F0F0F0E1LL, 0xF0F0F0F0E2LL, 0xF0F0F0F0E3LL};
int num_msg = 0;
int sndAckBuf[2] = {1,-1};
int valor_radar = 0;
int enviar = 0;
char m_send[128] = "";
String msg;
int longitud = 0;

void setup(void)
{
  Serial.begin(9600);;
  pinMode(radar, INPUT);
  radio.begin();
  radio.openReadingPipe(1,pipe [0]);
  radio.openReadingPipe(2,pipe [1]);
  radio.openReadingPipe(3,pipe [2]);
  radio.setAutoAck(1);
  radio.enableDynamicPayloads();
  radio.enableAckPayload();
  radio.startListening();
  #if defined(__AVR_ATtiny85__) && (F_CPU == 16000000)
  clock_prescale_set(clock_div_1);
#endif
  // END of Trinket-specific code.

  pixels.begin();
}
 
void loop(void){
val_LDR = analogRead(LDR);
pixels.clear();
if (radio.available()){
    receive_msg();
    num_msg++;
}
if(val_LDR < 100){
    if(digitalRead(radar) == HIGH){
        time1 = millis();(*@\label{tiempo1}@*)
        while(digitalRead(radar) == HIGH){
        enviar = enviar +1;
        if(enviar == 1){
            for(int i=0; i<NUMPIXELS; i++) {
              pixels.setPixelColor(i, pixels.Color(255, 255, 255));
              pixels.setBrightness(80); 
              pixels.show();
            }
        }
    }
    time2 = millis();(*@\label{tiempo2}@*)
    vel = 5/((time2 - time1)/1000);
    send_message(); 
    }
    for(int i=0; i<NUMPIXELS; i++) { (*@\label{bucle2}@*)
        pixels.setPixelColor(i, pixels.Color(255, 255, 255));
        pixels.setBrightness(20);
        pixels.show();
    }
}else{
    for (int i = NUMPIXELS; i> 0; i--){
        pixels.setBrightness(0);
        pixels.show();
        enviar = 0;
    }
  }
}

void receive_msg(){
String keyJson;
String valueJson;
mensaje_recibido = "";
radio.read(data, 128);
for (byte i = 0; i < sizeof(data); i++){
 mensaje_recibido = mensaje_recibido + String(data[i]);
}
radio.writeAckPayload(1, &sndAckBuf, sizeof(sndAckBuf)); 
radio.writeAckPayload(0, &sndAckBuf, sizeof(sndAckBuf));
radio.writeAckPayload(2, &sndAckBuf, sizeof(sndAckBuf));
    
deserializeJson(doc, mensaje_recibido);
JsonObject obj = doc.as<JsonObject>();
for (JsonPair mensaje_recibido : obj){
    keyJson = mensaje_recibido.key().c_str();
    valueJson = mensaje_recibido.value().as < char * > ();
    if (keyJson == /*KEY*/){
      emer = true;
    }
    while(keyJson == /*KEY*/ and emer == true){
        if(valueJson == /*Value*/){ 
            for(int i=0; i<NUMPIXELS; i++) { 
              pixels.setPixelColor(i, pixels.Color(0, 0, 255));
              pixels.setBrightness(100);
              pixels.show();
            }
        }
        if(valueJson == /*Value*/){ 
            for(int i=0; i<NUMPIXELS; i++) {
              pixels.setPixelColor(i, pixels.Color(255, 0, 0));
              pixels.setBrightness(100);
              pixels.show();
            }
        }
        receive_msg();  
        if(valueJson == /*Value*/){
            emer = false;
            for(int i=0; i<NUMPIXELS; i++) {
                pixels.setPixelColor(i, pixels.Color(255, 255, 255));
                pixels.setBrightness(20);
                pixels.show();
                return;
            }
        }
    }
    if(keyJson == /*Key*/){ 
        if(valueJson == /*Value*/){
            for(int i=0; i<NUMPIXELS; i++) {
                pixels.setPixelColor(i, pixels.Color(255, 255, 255));
                pixels.setBrightness(80);
                pixels.show();
            }
        }
    }
}
void send_message() {
if (vel < 50){(*@\label{ifvel}@*)
doc["Enciende"]= String(myId+1);
serializeJson(doc, msg);
doc.clear();
longitud = msg.length()+1;
msg.toCharArray(m_send, longitud);
msg = "";
radio.openWritingPipe(pipe[0]);
 if(radio.write(m_send, sizeof (m_send))){
  while (radio.isAckPayloadAvailable()) {
   int32_t msgSize = radio.getDynamicPayloadSize(); 
   byte msgIn[msgSize];     
   }
  }
radio.startListening();    
}else{
doc[/*KEY*/]= String(myId+1);
doc[/*KEY*/]= String(myId+1);
serializeJson(doc, msg);
doc.clear();
longitud = msg.length()+1;
msg.toCharArray(m_send, longitud);
msg = "";
radio.openWritingPipe(pipe[/*value pipe*/]);
 if(radio.write(m_send, sizeof (m_send))){
  while (radio.isAckPayloadAvailable()) {
   int32_t msgSize = radio.getDynamicPayloadSize(); 
   byte msgIn[msgSize];      
   }
 }
}
 
       
