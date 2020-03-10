#include "IntlerCloud.h"

Cloud::Cloud()
{
  login = password = device = "";
  
  IPAddress intlerCloud(79, 143, 30, 97);
  server = &intlerCloud;

  interval = DEFAULT_INTERVAL;
}

Cloud::Cloud(String login, String password, String device)
{
  this->login = login;
  this->password = password;
  this->device = device;

  IPAddress* intlerCloud = new IPAddress(79, 143, 30, 97);
  // IPAddress* intlerCloud = new IPAddress(192,168,0,2);
  server = intlerCloud;

  interval = DEFAULT_INTERVAL;
  receivedOrders = NULL;
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

void Cloud::sendValue(String name, double value) {

  if (sensorsList == NULL) {
    SensorValue* newSensorValue = new SensorValue;
    newSensorValue->name = name;
    newSensorValue->value = value;
    newSensorValue->next = NULL;

    sensorsList = newSensorValue; 
    return;
  }

  SensorValue* iter = sensorsList;
  while (iter->next != NULL) {
    if ((iter->name).equals(name)) {
      iter->value = value;
      return;
    }
    iter = iter->next;
  }

  if (name.equals(iter->name))
    iter->value = value;
  else {
    SensorValue* newSensorValue = new SensorValue;
    newSensorValue->name = name;
    newSensorValue->value = value;
    newSensorValue->next = NULL;
    (iter->next) = newSensorValue;
  }
}

void Cloud::addSensorValue(SensorValue* value) {
  if (sensorsList == NULL) {
    sensorsList = value; 
    return;
  }

  SensorValue* iter = sensorsList;
  while (iter->next != NULL) {
    if ((iter->name).equals(value->name)) {
      iter->value = value->value;
      return;
    }
    iter = iter->next;
  }

  (iter->next) = value;
}

String Cloud::getRequestBody() {
  String requestBody = "{\"login\":\"" + login + "\",";
  requestBody += "\'password\':\"" + password + "\",";
  requestBody += "\"deviceName\":\"" + device + "\",";
  requestBody += "\"deviceType\":\"Arduino\",\"sensorsValue\":{";

  SensorValue* iter = sensorsList;
  if (iter == NULL) 
    requestBody += "}";
  else {
    while (iter->next != NULL) {
      requestBody += "\"" + String(iter->name) + "\":" + String(iter->value) + "," ;
      iter = iter->next;
    }
    requestBody += "\"" + String(iter->name) + "\":" + String(iter->value) + "}";
  }
  requestBody += ", \"ordersAccepted\":"+ getOrdersString() +"}";
  
  return requestBody;
}

void Cloud::sendRequest() {
  EthernetClient client;
  String data = getRequestBody();
  if (client.connect(*server, 8080)) {
    Serial.println("connected");
    client.println("POST " + String(URL) + " HTTP/1.1");
    client.println("Host: 79.143.30.97:8080");
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

  clearOrders();
  clearSensorsValues();

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
  Serial.println("try connect");
  byte mac[] = { 0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xEA };

  IPAddress generatedSelfId(192,168,0,54);
  generatedIp = &generatedSelfId;
  Ethernet.begin(mac, *generatedIp);
  
delay(4000);
  Serial.println("Connected");

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
  orderName.trim();
  String val = str.substring(str.indexOf(":") + 1);
  val.trim();
  
  addNewOrder(orderName);
  double doubleVal = stringToDouble(val);

  Command* iter = commands;
  if (iter == NULL)
    return;
  do {
    if (orderName.equals(iter->name))
      (iter->procedure)(doubleVal); 
    iter = iter->next;
  } while ((iter != NULL));
}

void Cloud::addNewOrder(String orderName) {
  Order* newOrder = new Order;
  newOrder->name = orderName;
  newOrder->next = NULL;
  
  Order* iter = receivedOrders;
  if (iter == NULL)
    receivedOrders = newOrder; 
  else {
    while (iter->next != NULL) 
      iter = iter->next;
  
    iter->next = newOrder;
  }
}

double Cloud::stringToDouble(String str) {
  char floatbufVar[32];
  str.toCharArray(floatbufVar,sizeof(floatbufVar));
  
  return atof(floatbufVar);
}

void Cloud::addCommand(String name, void (*procedure)(double)) {
  Command* newCommand = new Command;
  newCommand->name = name;
  newCommand->procedure = procedure;
  newCommand->next = NULL;

  Command* iter = commands;
  if (iter == NULL)
    commands = newCommand; 
  else {
    while (iter->next != NULL) 
      iter = iter->next;
  
    iter->next = newCommand;
  }
}
 

String Cloud::getOrdersString() {
  String result = "[";
  Order* iter = receivedOrders;
  if (iter != NULL) {
    result += "\"" + iter->name + "\"";
    while (iter->next != NULL) {
      iter = iter->next;
      result += ",\"" + iter->name + "\"";
    }
  }
  result.concat("]");
  
  return result;
}

void Cloud::clearOrders() {
  Order* iter = receivedOrders;
  Order* deletedOrder;
  while (iter != NULL) {
    deletedOrder = iter;
    iter = iter->next;
    delete(deletedOrder);
  };
  receivedOrders = NULL;
}

void Cloud::clearSensorsValues() {
  SensorValue* iter = sensorsList;
  SensorValue* deletedValue;
  while (iter != NULL) {
    deletedValue = iter;
    iter = iter->next;
    delete(deletedValue);
  };
  sensorsList = NULL;
}

void Cloud::setInterval(int interval) {
  if (interval < 5000)
    this->interval = 5000;
  else
    this->interval = interval;

}
