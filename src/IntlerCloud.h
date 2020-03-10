#ifndef IntlerCloud_h
#define IntlerCloud_h

// #include "WProgram.h"
#include "Arduino.h"
#include <Ethernet2.h>

#define DEFAULT_INTERVAL 10000
#define URL "/intler-iot/send-device-data"

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

    void connect();
    void run();
    void setInterval(int interval);
    
    void sendValue(String name, double value);
    void addCommand(String name, void (*orderFunction)(double));
  private:
    String login;
    String password;
    String device;

    Order* receivedOrders;
    Command* commands;
    SensorValue* sensorsList;

    IPAddress* server;
    IPAddress* generatedIp;

    long requestTiming;
    int interval;

    void addSensorValue(SensorValue* value);
    void clearSensorsValues();

    bool waitResponce;
    EthernetClient* bufClient;
    void sendRequest();
    String getRequestBody();
    String getOrdersString();
    void parseHttpResponce(String responce);

    void executeOrder(String orderStr);
    void addNewOrder(String orderName);
    double stringToDouble(String doubleValue);
    void clearOrders();
};

#endif
