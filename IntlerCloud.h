#ifndef IntlerCloud_h
#define IntlerCloud_h

// #include "WProgram.h"
#include "Arduino.h"
#include <Ethernet2.h>

#define DEFAULT_INTERVAL 10000

struct Command {
  String name;
  void (*procedure)(double);
  Command* next;
};

struct Order {
  String name;
  Order* next;
};

struct SensorValue {
  String name;
  double value;
  SensorValue *next;
};

class Cloud
{
  public:
    Cloud();
    Cloud(String login, String password, String device);
    
    void setLogin(String login);
    void setPassword(String password);
    void setDevice(String device);
    
    void setInterval(int interval);
    void connect();
    void run();
    bool updated();
    void sendValue(String name, double value);
    void addCommand(String name, void (*orderFunction)(double));
    void printValuesList();
    void parseHttpResponce(String responce);
    String getOrdersString();
  private:
    Command* commands;
    Order* receivedOrders;
    long requestTiming;
    int interval;
    IPAddress* server;
    IPAddress* generatedIp;
    String login;
    String password;
    String device;
    bool isUpdated;
    void sendRequest();
    String getRequestBody();
    SensorValue *sensorsList;
    void addSensorValue(SensorValue* value);
//    void parseHttpResponce(String responce);
    void executeOrder(String orderStr);
    double stringToDouble(String doubleValue);
    void addNewOrder(String orderName);
    void clearOrders();
};

#endif
