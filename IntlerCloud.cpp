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

