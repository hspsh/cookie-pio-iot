#include <Arduino.h>
#include <LeifHomieLib.h>

#if defined(ARDUINO_ARCH_ESP8266)
#include <ESP8266WiFi.h>
#else
#include "WiFi.h"
#endif

#include <ArduinoOTA.h>
#include <initializer_list>
#include <string>

#include <Button.h>

#include "pinDefs.h"
#include "secrets.h"
#include "config.h"
#include "commonFwUtils.h"

HomieDevice homie;
HomieProperty *pPropBuzzer = NULL;
HomieProperty *pPropButton1 = NULL;
HomieProperty *pPropButton2 = NULL;
HomieProperty *pPropButton3 = NULL;

Button* buttons[3];

void init_homie_button(HomieNode* pNode, int pin_num, String id){
  Button* pButton = new Button(pin_num, INPUT);
  HomieProperty *pProperty = pNode->NewProperty();

  pProperty->strFriendlyName = id;
  pProperty->strID = id;
  pProperty->datatype = homieBool;
  pProperty->SetBool(pButton->isPressed());
  pProperty->strFormat = "";
  pButton->onChange([pButton,pProperty]() {
    pProperty->SetBool(pButton->isPressed());
  });
}

void init_buzzer(HomieNode* pNode, HomieProperty* pProperty, String id){
  pProperty = pNode->NewProperty();
  pProperty->strFriendlyName = "Annoying " + id;
  pProperty->strID = id;
  pProperty->SetRetained(true);
  pProperty->SetSettable(true);
  pProperty->datatype = homieBool;
  pProperty->SetBool(false);
  pProperty->strFormat = "";
  //callback is actually not needed cause buzzer is handled in loop
  // pPropBuzzer->AddCallback([](HomieProperty *pSource) {
  // 	Serial.printf("%s is now %s\n",pSource->strFriendlyName.c_str(),pSource->GetValue().c_str()); 
  // });
}

void init_homie_stuff(HomieDevice* pHomie){
  pHomie->strFriendlyName = friendlyName;
  #if defined(APPEND_MAC_TO_HOSTNAME)
    char out[20];
    sprintf(out, "%s-%X",hostname, WiFi.macAddress().c_str());
    pHomie->strID = out;
  #else
    pHomie->strID = hostname;
  #endif
  pHomie->strID.toLowerCase();

  pHomie->strMqttServerIP = "192.168.88.170";
	pHomie->strMqttUserName = MQTT_USERNAME;
	pHomie->strMqttPassword = MQTT_PASSWD;
  pHomie->Init();
}

void setup() {
  Serial.begin(115200);
  // client.begin(, IOT_WIFI_PASSWD);
  pinMode(PIN_LED, OUTPUT);
  pinMode(PIN_BUZZER, OUTPUT);

  WiFi.mode(WIFI_STA);
  WiFi.begin(IOT_WIFI_NAME, IOT_WIFI_PASSWD);

  while (WiFi.status() != WL_CONNECTED) {
    static bool flag = false;
    digitalWrite(PIN_LED,flag);
    flag = !flag;

    Serial.print(".");
    delay(3000);    
  }

  begin_hspota();    
  HomieNode *pNode = homie.NewNode();
  pNode->strID = "properties";
  pNode->strFriendlyName = "Properties";

  init_homie_button(pNode, PIN_BUTTON_1, "slot-1");
  init_homie_button(pNode, PIN_BUTTON_2, "slot-2");
  init_homie_button(pNode, PIN_BUTTON_3, "slot-3");

  init_buzzer(pNode,pPropBuzzer, "buzzer");  
  init_homie_stuff(&homie);  
}

void loop() {
  if(homie.IsConnected()){
    handle_io_pattern(PIN_LED,PATTERN_HBEAT);
  } else {
    handle_io_pattern(PIN_LED,PATTERN_ERR);
  }

  if (pPropBuzzer->GetValue() == "true")
  {
    handle_io_pattern(PIN_BUZZER,PATTERN_HBEAT);
  } else {
    handle_io_pattern(PIN_BUZZER,PATTERN_NONE);
  }

  ArduinoOTA.handle();
  homie.Loop();
  delay(100);
}
