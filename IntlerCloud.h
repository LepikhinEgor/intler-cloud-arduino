#ifndef IntlerCloud_h
#define IntlerCloud_h

struct SensorValue {
  String name;
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
    bool updated();
    void sendValue(String name, double value, bool constantly);
    void addCommand(String name, void (*orderFunction)(double));
  private:
    String login;
    String password;
    String device;
    bool isUpdated;
    void sendRequest();
    String prepareUrl();
    SensorValue *sensorsList;
    void addSensorValue(SensorValue* value);
};

 
#endif
