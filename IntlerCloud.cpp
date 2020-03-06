#include "InlerCloud.h"

Cloud::Cloud()
{
  login = password = device = "";
}

Cloud::Cloud(String login, String password, String device)
{
  this->login = login;
  this->password = password;
  this->device = device;
}

void Cloud::setLogin(String login) {
  this->login = login;
}

void Cloud::setPassword(String password) {
  this->password = password;
}

void Cloud::setDevice(String device) {
  this->device = device;
}
bool Cloud::updated() {
  return isUpdated;
}

void Cloud::sendValue(String name, double value, bool constantly) {
  SensorValue newSensorValue;
  newSensorValue.name = name;
  newSensorValue.value = value;
  newSensorValue.constantly = constantly;
  
  addSensorValue(&newSensorValue);
}

void Cloud::addSensorValue(SensorValue* value) {
  if (sensorsList == NULL) {
    *sensorsList = *value;
    return;
  }

  SensorValue* iter = sensorsList;
  while (iter->next != NULL) 
    iter = iter->next;

  *(iter->next) = *value;
}

String Cloud::getRequestBody() {
  String requestBody = "{\"login\":\"" + login + "\",";
  requestBody += "\"password\":\"" + password + "\",";
  requestBody += "\"deviceName\":\"" + device + "\",";
  requestBody += "\"deviceType\":\"Arduino\",\"sensorsValue\":{";
  
  SensorValue* iter = sensorsList;
  while (iter->next != NULL) {
    requestBody += "\"" + iter->name + "\":" + iter->value + "," ;
    iter = iter->next;
  }
  requestBody += "\"" + iter->name + "\":" + iter->value + "}, \"ordersAccepted\":[]}";
  
  return requestBody;
}


void Cloud::sendRequest() {
  EthernetClient client;
  String data = getRequestBody();
  if (client.connect(server, 8080)) {
    Serial.println("connected");
    client.println("POST /intler_iot_war_exploded/send-device-data HTTP/1.1");
    client.println("Host: 192.168.0.42:8080");//????????????????
    client.println("Content-Type: application/json");
    client.println("Connection: close");
    client.print("Content-Length: ");
    client.println(data.length());
    client.println();
    client.print(data);
    client.println();
  }
  else {
    Serial.println("connection failed");
  }

  String cloudInput;
  while (client.available()) {
    char c = client.read();
    cloudInput.concat(c);
  }
      Serial.println(cloudInput);
  parseHttpResponce(cloudInput);
  cloudInput = "";
}

void Cloud::connect() {
  byte mac[] = { 0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED };
  
  if (Ethernet.begin(mac) == 0) {
    Serial.println("Failed to configure Ethernet using DHCP");
    IPAddress generatedSelfId(192,168,0,54);
    generatedIp = &generatedSelfId;
    Ethernet.begin(mac, generatedIp);
  }
}

void Cloud::run() {
  if (millis() - requestTiming > interval) {
    sendRequest();

    requestTiming = millis();
  }
}

void Cloud::parseHttpResponce(String responce) {
  String body = responce.substring(responce.indexOf("{"),responce.indexOf("}") + 1); //TODO

  int bodyPos = body.indexOf("{");

  String curOrderStr = body.substring(1); 
  while(true) {
    String order;
    int commaPos = curOrderStr.indexOf(",");
    if (commaPos == -1) {
      order = curOrderStr.substring(0, curOrderStr.indexOf("}"));
      if (curOrderStr.indexOf("}") < 2) {
        Serial.println("No command");
        return;
        }
      executeOrder(order);
      break;
    } else {
      order = curOrderStr.substring(0, curOrderStr.indexOf(","));
      executeOrder(order);
      curOrderStr = curOrderStr.substring(curOrderStr.indexOf(",") + 1);
    }
  }
}



void Cloud::executeOrder(String str) {
  int nameEndPos = str.substring(1).indexOf("\"");
  String orderName = str.substring(1, nameEndPos + 1);
  String val = str.substring(str.indexOf(":") + 1);
  Serial.println("name " + orderName + " val " + val);
  
  Order newOrder;
  newOrder.name = orderName;
  Order* iter = receivedOrders;
  while (iter->next != NULL) 
    iter = iter->next;

  *(iter->next) = newOrder;
  

  float floatVar;
  char floatbufVar[32];
  val.toCharArray(floatbufVar,sizeof(floatbufVar));
  floatVar=atof(floatbufVar);

  int intVal = 0;
  intVal = (int)floatVar;
  if (ceil(floatVar) - floatVar < 0.001)
    intVal = ceil(floatVar);

  Serial.println("clouded val " + String(intVal));
  
  if (intVal = 1)  {
    Serial.println("lightOn");
    digitalWrite(8, false);
    }
  if (intVal = 0) {
    Serial.println("lightOff");
    digitalWrite(8, true); 
    }
}

