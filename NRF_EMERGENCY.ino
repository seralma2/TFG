#include <nRF24L01.h>
#include <RF24.h>
#include <RF24_config.h>
#include <SPI.h>
#include <ArduinoJson.h>
#include <ArduinoJson.hpp>
DynamicJsonDocument doc(1024);
String msg;
const int pinCE = 9;
const int pinCSN = 10;
int boton = 4;
int boton_fin = 7;
RF24 radio(pinCE, pinCSN);
int longitud = 0;
// Single radio pipe address for the 2 nodes to communicate.
const uint64_t pipe = 0xF0F0F0F0E3LL;
char m_send[128] = "";
char data[16]="" ;
//seleccionador de pipe
int contador;
void setup(void)
{
  Serial.begin(9600);
  radio.begin();
  
  //radio.openWritingPipe(pipe[0]);
  pinMode(boton, INPUT);
  pinMode(boton_fin, INPUT);
  radio.openReadingPipe(1, pipe);
  radio.enableDynamicPayloads();
  radio.enableAckPayload();
  radio.startListening();
  
}
 
void loop(void)
{
  contador = 1;
  //Serial.println(value_boton);
  int value_boton = digitalRead(boton);
  int value_boton_fin = digitalRead(boton_fin);
  //Serial.println(value_boton_fin);
  if(value_boton == HIGH && contador == 1 ){
    msg_ini();
  }
  if(value_boton_fin == HIGH ){
    msg_fin();
  }
  //solo acepta []
  
  if(longitud > 0){
      radio.openWritingPipe(pipe);
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
      longitud = 0;
    }
}
void msg_ini(){
    radio.stopListening();
  //radio.write(data, 16);
  //Serial.println("he entrado");
  doc["Emergency"]= "2";
  //doc["2"]= "Hello";
  serializeJson(doc, msg);
  Serial.println(msg);
  doc.clear();
  longitud = msg.length()+1;
  //Serial.print ("La longitud es :");
  //Serial.println(longitud);
  //convertidos el string en cadena para poder enviar
  msg.toCharArray(m_send, longitud);
  msg = "";
    //Serial.println("Se ha serializado");
    contador = 0;
  }

void msg_fin(){
    radio.stopListening();
  //radio.write(data, 16);
  Serial.println("he entrado");
  doc["Emergency"]= "FIN";
  //doc["2"]= "Hello";
  serializeJson(doc, msg);
  Serial.println(msg);
  doc.clear();
  longitud = msg.length()+1;
  Serial.print ("La longitud es :");
  Serial.println(longitud);
  //convertidos el string en cadena para poder enviar
  msg.toCharArray(m_send, longitud);
  msg = "";
    Serial.println("Se ha serializado");
  }
