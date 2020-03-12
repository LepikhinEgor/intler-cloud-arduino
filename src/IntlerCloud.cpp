#include "IntlerCloud.h"

Cloud::Cloud()
{
  login = password = device = "";
  
  server = new IPAddress(79, 143, 30, 97);

  interval = DEFAULT_INTERVAL;
}

Cloud::Cloud(String login, String password, String device)
{
  this->login = login;
  this->password = password;
  this->device = device;

  server = new IPAddress(79, 143, 30, 97);
  // IPAddress* intlerCloud = new IPAddress(192,168,0,2);

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
  requestBody += "\"password\":\"" + password + "\",";
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
  if (!waitResponce) {
    bufClient = new EthernetClient;
    String data = getRequestBody();
    // Serial.println(data);
    if ((*bufClient).connect(*server, 8080)) {
      // Serial.println("connected");
      (*bufClient).println("POST /intler-iot/send-device-data HTTP/1.1");
      (*bufClient).println("Host: 192.0.0.2:8080");
      (*bufClient).println("Content-Type: application/json");
      (*bufClient).println("Connection: close");
      (*bufClient).print("Content-Length: ");
      (*bufClient).println(data.length());
      (*bufClient).println();
      (*bufClient).print(data);
      (*bufClient).println();
    }
    else {
      Serial.println("cannot connect to Intler cloud");
    }

    clearOrders();
    clearSensorsValues();

    requestTiming = millis();
    waitResponce = true;
  }

  if (millis() - requestTiming > interval) {
    waitResponce = false;

    String cloudInput = "";
    while ((*bufClient).available()) {
      char c = (*bufClient).read();
      cloudInput.concat(c);
    }

    parseHttpResponce(cloudInput.substring(cloudInput.indexOf("{"),cloudInput.indexOf("}") + 1));
    delete(bufClient);
  }
}

void Cloud::connect() {
  Serial.println("Connecting to internet...");
  byte mac[6];
  for (int i = 0; i < 6; i++)
    mac[i] = random(255);

  IPAddress generatedSelfId(192,168,0,54);
  generatedIp = &generatedSelfId;

  if (Ethernet.begin(mac) == 0) {
    Serial.println("Failed connection using DHCP");
    Ethernet.begin(mac, *generatedIp);
  } else {
    Serial.println("Success connected");
  }
  
delay(3000);
}

void Cloud::run() {
    sendRequest();
}

void Cloud::parseHttpResponce(String responce) {
  // String responce = responce.substring(responce.indexOf("{"),responce.indexOf("}") + 1); //TODO

  int bodyPos = responce.indexOf("{");

  String curOrderStr = responce.substring(1); 
  while(true) {
    String order;
    int commaPos = curOrderStr.indexOf(",");
    if (commaPos == -1) {
      order = curOrderStr.substring(0, curOrderStr.indexOf("}"));
      if (curOrderStr.indexOf("}") < 2) {
        //Serial.println("No command");
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

void Cloud::changeCloudIP(byte ip1, byte ip2, byte ip3, byte ip4) {
  delete(server);
  server = new IPAddress(ip1, ip2, ip3, ip4);
}
