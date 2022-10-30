#include <AsyncMqttClient.h>

AsyncMqttClient mqttClient;


class MqNode {
public:
  virtual void onInit(String topic) = 0;
};

class MqButton : public MqNode {
  Button button;
  String name;

public:
  MqButton(String name, uint8 port) : button(port, INPUT), name(name) {}

  void onInit(String topic) {
    topic.concat("/");
    topic.concat(name);

    mqttClient.publish(topic.c_str(), 0, true,
                       button.isPressed() ? "YES" : "NO");
    button.onChange([topic, this]() {
      mqttClient.publish(topic.c_str(), 0, true,
                         button.isPressed() ? "YES" : "NO");
    });
  }
};

class MqIoNotif : public MqNode {
  String name;
  uint8 io_num;

public:
  MqIoNotif(String name, uint8 port) : name(name) {
    io_num = port;
  }

  void onInit(String topic) {
    topic.concat("/");
    topic.concat(name);

    mqttClient.onMessage(
      [this](char* topic, char* payload, AsyncMqttClientMessageProperties properties, size_t len, size_t index, size_t total){
        //do stuff at some point
      });
  }
};

class MqStringProperty : public MqNode {
  String name, value;

public:
  MqStringProperty(String name, String value) : name(name), value(value) {}

  void onInit(String topic) {
    topic.concat("/");
    topic.concat(name);

    mqttClient.publish(topic.c_str(), 0, true, value.c_str());
  }
};

class MqStatsAggregate : public MqNode {
  MqStringProperty mqIpAddr;

public:
  MqStatsAggregate() : mqIpAddr("ipAddr","2137") {}

  void onInit(String topic) {
    topic.concat("/");
    topic.concat("stats");

    // mqttClient.publish(topic.c_str(), 0, true, "idk");
    mqIpAddr.onInit(topic);
  }
};


template<int SIZE> class MqBranch : public MqNode {
  String name;
  std::array<MqNode*, SIZE> mqNodes;

public:
  MqBranch(String name, std::array<MqNode*, SIZE> mqNodes) : name(name), mqNodes(mqNodes) {}

  void onInit(String topic) {
    topic.concat("/");
    topic.concat(name);

    for(auto mqNode: mqNodes){
        mqNode->onInit(topic);
    }
  }
};

template<int SIZE> class MqRoot {
  String name;
  std::array<MqNode*, SIZE> mqNodes;

public:
  MqRoot(String name, std::array<MqNode*, SIZE> mqNodes) : name(name), mqNodes(mqNodes) {}

  void onInit() {
    for(auto mqNode: mqNodes){
        mqNode->onInit(name);
    }
    // root may subscribe to everything for debug purposes
    // String topicWildcard = name+"/#";
    // mqttClient.subscribe(topicWildcard.c_str(), 2);
  }

  void handleMessage(const char* topic, const char* payload, AsyncMqttClientMessageProperties properties, size_t len, size_t index, size_t total) {
    // Serial.println("Service %s received %s",name.c_str(),(char*)topic);
  }
};

// String deviceName = String(zESP.getChipId());
String deviceName = "triton one";

MqRoot<1> *mqTrittonService = 
    new MqRoot<1>("homie", {
      new MqBranch<2>(deviceName, {
        new MqBranch<3>("nuttons", {
          new MqButton("A", 14),
          new MqButton("B", 13), 
          new MqButton("C", 12)
        }),
        new MqStatsAggregate()
    })
  });


void attachMessageHandler(AsyncMqttClient& client){
  mqttClient.onMessage(
    [](char* topic, char* payload, AsyncMqttClientMessageProperties properties, size_t len, size_t index, size_t total){
      mqTrittonService->handleMessage(topic,payload,properties,len,index,total);
    });
}