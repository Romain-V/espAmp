#include <ESP8266WiFi.h>
#include <BlynkSimpleEsp8266_SSL.h>

// You should get Auth Token in the Blynk App.
// Go to the Project Settings (nut icon).
char auth[] = "yourauthtoken";

// Your WiFi credentials.
// Set password to "" for open networks.
char ssid[] = "yourssid";
char pass[] = "yourpassword";

// Attach virtual serial terminal to Virtual Pin V1
WidgetTerminal terminal(V1);

String message;

BLYNK_WRITE(V1)
{
  Serial.print(param.asStr());
  Serial.println();
  //Used to send commands directly from Blynk terminal
}


BLYNK_WRITE(V0){
  int pinData = param.asInt();
  setVolume(pinData);
}

BLYNK_WRITE(V2){
  int buttonState = param.asInt();
  if(buttonState==1){
    turnOn();
  }
  else if (buttonState==0){
    turnOff();
  }
}

BLYNK_WRITE(V4){
  int pinData = param.asInt();
  setAudioMode(pinData);
}
BLYNK_WRITE(V3){
  int pinData = param.asInt();
  setAudioInput(pinData);
}

void setup()
{
  //link to amplifier
  Serial.begin(9600);

  Blynk.begin(auth, ssid, pass);
  // You can also specify server:
  //Blynk.begin(auth, ssid, pass, "blynk-cloud.com", 80);
  //Blynk.begin(auth, ssid, pass, IPAddress(192,168,1,100), 8080);
}


void setAudioInput(int audinput){
//I only use a few optical inputs, feel free to add your own
  Serial.print("SI");
  switch(audinput){
    case 1:
    Serial.print("DBS/SAT");
    break;
    case 2:
    Serial.print("TV");
    break;
    case 3:
    Serial.print("VDP");
    break;
  }
  Serial.println();
}

void setAudioMode(int audimod){
//same as with inputs, I use only a few modes
  Serial.print("MS");
  switch(audimod){
    case 1:
    Serial.print("DIRECT");
    break;
    case 2:
    Serial.print("STEREO");
    break;
    case 3:
    Serial.print("DOLBY DIGITAL");
    break;
    case 4:
    Serial.print("DTS SURROUND");
    break;
    case 5:
    Serial.print("MATRIX"); //Could be any other, set to MATRIX cause first one I could think of
    break;
  }
  Serial.println();
  
}


void setVolume(int volume){
//Amp takes values from 0 to 99 but I'd rather display 1 to 100
  volume = volume - 1;
  String vol = String(volume);
  Serial.print("MV");
  Serial.print(vol);
  Serial.print('\r');
}

void turnOn(){
  Serial.print("PWON");
  Serial.print('\r');
}

void turnOff(){
  Serial.print("PWSTANDBY");
  Serial.print('\r');
}

void messageParse(String mesg){
  if (mesg[0] =='M' & mesg[1] == 'V'){
    //Volume change
      int volume = mesg.substring(2).toInt();
      if (volume > 100){
        volume = int(volume/10);
      }
      volume = volume + 1;
      terminal.print("Volume manually set to :");
      terminal.print(String(volume));
      terminal.println();
      Blynk.virtualWrite(V0, volume);
    }
    else if(mesg[0] == 'M' & mesg[1] == 'S'){
      //Audio mode switch
      if (mesg.length() == 8){
        if (mesg[2]=='D'){
          Blynk.virtualWrite(V4,1);
        }
        else if(mesg[2]=='S'){
          Blynk.virtualWrite(V4,2);
        }
        else {
          Blynk.virtualWrite(V4,5);
        }
      }
      else if(mesg.substring(2,7).compareTo("DOLBY")=='0'){
        Blynk.virtualWrite(V4,3);
      }
      else if (mesg.substring(2,5).compareTo("DTS")=='0'){
        Blynk.virtualWrite(V4,4);
      }
      else{
        Blynk.virtualWrite(V4,5);
      }
    }
    else if (mesg[0] == 'P' & mesg[1] == 'W'){
      //Power switch
      if (mesg[3] == 'O'){
         Blynk.virtualWrite(V2,HIGH); //Switching on
      }
      else{
         Blynk.virtualWrite(V2,LOW); //Standing by
      }
    }else if (mesg[0] == 'S' & mesg[1] == 'I'){
      //Input Switch
      String sub = mesg.substring(2,4);
      if (sub.compareTo("DB")==0){
        Blynk.virtualWrite(V3, 1);
      }
      else if (sub.compareTo("TV")==0){
        Blynk.virtualWrite(V3, 2);
      }
      else if (sub.compareTo("VD")==0){
        Blynk.virtualWrite(V3, 3);
      }
      else{
        Blynk.virtualWrite(V3, 4);
      }
    }
    else{
      terminal.print(mesg);
      terminal.println();
    }
}

void loop()
{
  Blynk.run();
  if(Serial.available()){
    while(Serial.available()){
      message = Serial.readStringUntil('\r');
      messageParse(message);
    }
  }
  delay(500);
}
