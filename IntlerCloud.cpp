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

//  while (client.available()) {
//    char c = client.read();
//    Serial.print(c);
//    cloudInput.concat(c);
//  }
//  executeCloudOrders(cloudInput.substring(cloudInput.indexOf("{"),cloudInput.indexOf("}") + 1));
//  cloudInput = "";
}

