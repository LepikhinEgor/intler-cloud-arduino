#ifndef IntlerCloud_h
#define IntlerCloud_h

#define DEFAULT_INTERVAL 5000

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
  char name[30];
  double value;
  bool constantly;
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
    bool updated();
    void sendValue(String name, double value, bool constantly);
    void addCommand(String name, void (*orderFunction)(double));
    void printValuesList();
    void parseHttpResponce(String responce);
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
};

#endif
