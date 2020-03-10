# IntlerCloud library
LibraryCloud.h is official library for connecting to Intler cloud by internet. This library allows you:

  - Send data to Intler cloud
  - Receive data from Intler Cloud
  - Add data handlers as simple Arduino functions

### Setup
First of all you need download this library  and include it to your scetch:
```cpp
#include <IntlerCloud.h>
```
Next you need create object Cloud with your account credentials:

```cpp
Cloud cloud("your_username", "your_password", "device_name");
```
In function *setup()* you can configure *Cloud*. For example you can change username/password/deviceName by functions:

```cpp
cloud.setLogin("new_username");
cloud.setPassword("new_password");
cloud.setDevice("new_device");
```
To send and receive data from Intler cloud, library send HTTP request to server every 10 seconds by default. You can change this value:
```cpp
cloud.setInterval(7000);
```
Now it wil be sending request every 7 seconds. Minimal time for timer is 5 seconds.
To connect to internet and Intler cloud you need call function *connect()*:
```cpp
cloud.connect();
```
To get started exchange data with cloud you need call function *run()* in ***loop()***:
```cpp
intlerCloud.run();
```

### Send
To send data to Intler cloud you can use function *`sendValue(String name, double value)`*:
```cpp
cloud.sendValue("temperature", 22.5);
```
After data sending to server it's wil be deleted from Arduino memory. If you want hourly send data, you need call this function multiply times. For example in function *loop()*

### Receive
Library also allow receive data from cloud. By default received values will be ignored. If you want to handle it, you need create fucntion-handler. This function must be ***void*** and have one argument with type ***double*** for example:
```cpp
void printValue(double value) {
  Serial.println("Received from Intler cloud value " + String(val));
}
```
Next you need call function addCommand() in ***setup()***:
```cpp
cloud.addCommand("music_volume", printValue);
```
It means that function printValue(double value) will be call, when Arduino received data with name "music_volume" from Intler cloud.

