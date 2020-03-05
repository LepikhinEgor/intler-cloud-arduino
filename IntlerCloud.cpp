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


