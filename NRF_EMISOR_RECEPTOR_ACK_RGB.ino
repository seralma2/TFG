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
int val_LDR = 1000;
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
 
void loop(void)
{
  val_LDR = analogRead(LDR);
  pixels.clear();
  if (radio.available())
  {
    receive_msg();
    num_msg++;

  }
  
  if(val_LDR < 100){
         if(digitalRead(radar) == HIGH){
          while(digitalRead(radar) == HIGH){
            enviar = enviar +1;
            if(enviar == 1){
              for(int i=0; i<NUMPIXELS; i++) {
                //Serial.println("Subo intensidad");
                pixels.setPixelColor(i, pixels.Color(255, 255, 255));
                pixels.setBrightness(80);
                pixels.show();
              }
              send_message();
              }
            }
         }
        for(int i=0; i<NUMPIXELS; i++) {
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
        //Serial.print(data[i]);
        mensaje_recibido = mensaje_recibido + String(data[i]);
      }
    //radio.stopListening();
     radio.writeAckPayload(1, &sndAckBuf, sizeof(sndAckBuf)); 
     radio.writeAckPayload(0, &sndAckBuf, sizeof(sndAckBuf));
     radio.writeAckPayload(2, &sndAckBuf, sizeof(sndAckBuf));
    
    deserializeJson(doc, mensaje_recibido);
    JsonObject obj = doc.as<JsonObject>();
    for (JsonPair mensaje_recibido : obj)
    {
    keyJson = mensaje_recibido.key().c_str();
    valueJson = mensaje_recibido.value().as < char * > ();
    if (keyJson == "Emergency"){
      emer = true;
    }
    while(keyJson == "Emergency" and emer == true){
        if(valueJson == String(myId)){
         for(int i=0; i<NUMPIXELS; i++) { // For each pixel...

          pixels.setPixelColor(i, pixels.Color(0, 0, 255));
          pixels.setBrightness(100);
          pixels.show();
          }
        }
        if(valueJson == String(myId - 1)){
          for(int i=0; i<NUMPIXELS; i++) { 
            pixels.setPixelColor(i, pixels.Color(255, 0, 0));
            pixels.setBrightness(100);
            pixels.show();}
        }
        if(valueJson == String(myId+1)){
          for(int i=0; i<NUMPIXELS; i++) { // For each pixel...
            pixels.setPixelColor(i, pixels.Color(255, 0, 0));
            pixels.setBrightness(100);
            pixels.show();
            }
        }
          receive_msg();  
    //}
    //if(keyJson == "FIN"){
      if(valueJson == "FIN"){
        emer = false;
          Serial.println(valueJson);
            for(int i=0; i<NUMPIXELS; i++) {
            pixels.setPixelColor(i, pixels.Color(255, 255, 255));
            pixels.setBrightness(20);
            pixels.show();
            return;
          }
        }
      }
  }
}
void send_message(){
  /*
  String mensaje_enviar;
  char m_send [128] = "";
  int longitud_enviar = 0;
  doc.clear();
   radio.stopListening();
    //Serial.println(F("Enviando mensaje-----"));
    doc["Enciende"] = String(myId);
    serializeJson(doc, mensaje_enviar);
    doc.clear();
    longitud_enviar = mensaje_enviar.length()+1;
    mensaje_enviar.toCharArray(m_send, longitud_enviar);
    mensaje_enviar = "";
    Serial.println(F("se ha serializado"));
      //seleccionamos el canal 0
        radio.openWritingPipe(pipe[0]);
        if(radio.write(m_send, sizeof (m_send))){
        Serial.println(m_send);
         /* while (radio.isAckPayloadAvailable()) {
           int32_t msgSize = radio.getDynamicPayloadSize();  //ack payload length is dynamic
            byte msgIn[msgSize];
            Serial.print(F("ACK: "));
            Serial.println(radio.read(&msgIn, msgSize));
      }
      Serial.println(F("FIN"));
      radio.startListening();
        }*/
           radio.stopListening();
  //radio.write(data, 16);
  //Serial.println("he entrado");
  doc["Enciende"]= "3";
  serializeJson(doc, msg);
  Serial.println(msg);
  doc.clear();
  longitud = msg.length()+1;
  //Serial.print ("La longitud es :");
  //Serial.println(longitud);
  //convertidos el string en cadena para poder enviar
  msg.toCharArray(m_send, longitud);
  msg = "";
  radio.openWritingPipe(pipe[0]);
      Serial.println(m_send);
      if(radio.write(m_send, sizeof (m_send))){
        Serial.println("el m_send es: ");
        Serial.print(m_send);
          while (radio.isAckPayloadAvailable()) {
            Serial.println("aqui entro??");
            int32_t msgSize = radio.getDynamicPayloadSize();  //ack payload length is dynamic
            byte msgIn[msgSize];
            Serial.print("ACK: ");
            
            Serial.println(radio.read(&msgIn, msgSize));
          }
      }
      Serial.println("FIN");
      radio.startListening();
    }
 
       
