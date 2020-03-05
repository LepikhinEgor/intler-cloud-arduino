#ifndef IntlerCloud_h
#define IntlerCloud_h

class Cloud
{
  public:
    Cloud();
    Cloud(String login, String password, String device);
    
    void setLogin(String login);
    void setPassword(String password);
    void setDevice(String device);
    
    void connect();
    void addValue(String name, double value);
    bool updated();
    void addCommand(String name, void (*orderFunction)(double));
  private:
    String login;
    String password;
    String device;
    bool isUpdated;
    void sendRequest();
    String prepareUrl();
};
 
#endif
