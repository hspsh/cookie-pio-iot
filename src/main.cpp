#include <Arduino.h>

#define USE_ASYNCMQTTCLIENT
#define ARDUINO_ARCH_ESP8266
#define HOMIELIB_VERBOSE
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

#include "pinouts.h"
#include "secrets.h"
#include "config.h"

HomieDevice homie;
// We'll need a place to save pointers to our created properties so that we can access them again later.
HomieProperty *pPropBuzzer = NULL;
HomieProperty *pPropButton1 = NULL;
HomieProperty *pPropButton2 = NULL;
HomieProperty *pPropButton3 = NULL;

Button *button_1 = new Button(PIN_BUTTON_1, INPUT);
Button *button_2 = new Button(PIN_BUTTON_2, INPUT);
Button *button_3 = new Button(PIN_BUTTON_3, INPUT);

void setup() {
  Serial.begin(115200);
  // client.begin(, IOT_WIFI_PASSWD);
  pinMode(PIN_LED, OUTPUT);
  pinMode(PIN_BUZZER, OUTPUT);

  WiFi.mode(WIFI_STA);
  WiFi.begin(IOT_WIFI_NAME, IOT_WIFI_PASSWD);
  while (WiFi.waitForConnectResult(3000) != WL_CONNECTED) {
    static bool flag = false;
    digitalWrite(PIN_LED,flag);
    flag = !flag;
  }

  ArduinoOTA.setPassword(ARDUINO_OTA_PASSWD);

  ArduinoOTA
    .onStart([]() {
      String type;
      if (ArduinoOTA.getCommand() == U_FLASH)
        type = "sketch";
      else // U_SPIFFS
        type = "filesystem";

      // NOTE: if updating SPIFFS this would be the place to unmount SPIFFS using SPIFFS.end()
      Serial.println("Start updating " + type);
    });
    ArduinoOTA.onEnd([]() {
      Serial.println("\nEnd");
    });

    ArduinoOTA.onProgress([](unsigned int progress, unsigned int total) {
      Serial.printf("Progress: %u%%\r", (progress / (total / 100)));
    });

    ArduinoOTA.onError([](ota_error_t error) {
      Serial.printf("Error[%u]: ", error);
      if (error == OTA_AUTH_ERROR) Serial.println("Auth Failed");
      else if (error == OTA_BEGIN_ERROR) Serial.println("Begin Failed");
      else if (error == OTA_CONNECT_ERROR) Serial.println("Connect Failed");
      else if (error == OTA_RECEIVE_ERROR) Serial.println("Receive Failed");
      else if (error == OTA_END_ERROR) Serial.println("End Failed");
    });

  ArduinoOTA.begin();

  {
    HomieNode *pNode = homie.NewNode();

    pNode->strID = "properties";
    pNode->strFriendlyName = "Properties";
    //		pNode->strType="customtype";

    HomieProperty *pProp;

    pPropBuzzer = pProp = pNode->NewProperty();
    pProp->strFriendlyName = "Annoying Buzzer";
    pProp->strID = "buzzer";
    pProp->SetRetained(true);
    pProp->SetSettable(true);
    pProp->datatype = homieBool;
    pProp->SetBool(false);
    pProp->strFormat = "";
    pProp->AddCallback([](HomieProperty *pSource) {
			//this property is settable. We'll print it into the console whenever it's updated.
			//you can set it from MQTT Explorer by publishing a number between 0-100 to homie/examplehomiedev/nodeid1/dimmer
			//but remember to check the *retain* box.
			Serial.printf("%s is now %s\n",pSource->strFriendlyName.c_str(),pSource->GetValue().c_str()); 
      digitalWrite(16, strcmp(pSource->GetValue().c_str(), "true") == 0 ? HIGH : LOW); 
    });

    pPropButton1 = pProp = pNode->NewProperty();
    pProp->strFriendlyName = "Slot 1";
    pProp->strID = "slot-1";
    pProp->datatype = homieBool;
    pProp->SetBool(button_1->isPressed());
    pProp->strFormat = "";
    button_1->onChange([]() {
      pPropButton1->SetBool(button_1->isPressed());
    });

    pPropButton2 = pProp = pNode->NewProperty();
    pProp->strFriendlyName = "Slot 2";
    pProp->strID = "slot-2";
    pProp->datatype = homieBool;
    pProp->SetBool(button_2->isPressed());
    pProp->strFormat = "";
    button_2->onChange([]() {
      pPropButton2->SetBool(button_2->isPressed());
    });

    pPropButton3 = pProp = pNode->NewProperty();
    pProp->strFriendlyName = "Slot 3";
    pProp->strID = "slot-3";
    pProp->datatype = homieBool;
    pProp->SetBool(button_3->isPressed());
    pProp->strFormat = "";
    button_3->onChange([]() {
      pPropButton3->SetBool(button_3->isPressed());
    });
  }

  homie.strFriendlyName = friendlyName;
  #if defined(APPEND_MAC_TO_HOSTNAME)
    char out[20];
    sprintf(out, "%s-%X",hostname, ESP.getChipId());
    homie.strID = out;
  #else
    homie.strID = hostname;
  #endif
  homie.strID.toLowerCase();

  homie.strMqttServerIP = "192.168.88.170";
	homie.strMqttUserName = MQTT_USERNAME;
	homie.strMqttPassword = MQTT_PASSWD;
  homie.Init();
}

void loop() {
  static int test = 0;
  static uint8_t heartbeat_pattern[] = {1,0,0,1,0,0,0,0,0,0,0,0,0};
  static uint8_t errcon_pattern[] = {1,0,1,0,1,0,1,1,1,0,0,0,0};
  
  uint8_t pattern_index = (test++)% sizeof(heartbeat_pattern);
  digitalWrite(PIN_LED, heartbeat_pattern[pattern_index]);
  if (pPropBuzzer->GetValue() == "true")
  {
    digitalWrite(PIN_BUZZER, heartbeat_pattern[pattern_index]);
  } else {
    digitalWrite(PIN_BUZZER, LOW);
  }
  if(!homie.IsConnected())
	{
    digitalWrite(PIN_BUZZER, errcon_pattern[pattern_index]);
  }
  ArduinoOTA.handle();
  homie.Loop();
  delay(100);
}
