// replace the Strings with your own settings and save it into config.h
// make shure not to push your own passwords on github by placing this file outside the git-repo and edit the Filepath in the code for the nodemcus 


#define WIFI_PASSWORD "your_Password_here"
#define WIFI_SSID "your_SSID_here"
#define MQTT_SERVER_IP "your mqtt server ip"
#define MQTT_USER_LEDSTRIP "mqtt user"       // if MQTT_USER_LEDSTRIP and MQTT_PASSWORD_LEDSTRIP are "" the LED-strip nodeMCU will try to connect without username and password
#define MQTT_PASSWORD_LEDSTRIP "mqtt password"
