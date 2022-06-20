#include <Arduino.h>
#include <Servo.h>
#include <IBMIOTF8266.h>

String user_html = "";
int                 cmdBaseLen = 10;

Servo servo;

char*               ssid_pfix = (char*)"Edge_ServoLabelX";

//unsigned long       lastPublishMillis = - pubInterval;

void publishData() {
    StaticJsonDocument<512> root;
    JsonObject data = root.createNestedObject("d");

// USER CODE EXAMPLE : command handling
    //String Casting
    data["state"] = String(servo.read());
// USER CODE EXAMPLE : command handling

    serializeJson(root, msgBuffer);
    client.publish(publishTopic, msgBuffer);
}

void handleUserCommand(JsonDocument* root) {
    JsonObject d = (*root)["d"];

    
// USER CODE EXAMPLE : status/change update
// code if any of device status changes to notify the change

    if(d.containsKey("drive")) {
        if (!strcmp(d["drive"], "on")) {
            servo.write(120); 
            publishData();
        } 
      //lastPublishMillis = - pubInterval;
    }

    if(d.containsKey("detect")) {
        if (!strcmp(d["detect"], "on")) {
            servo.write(0); 
            publishData();
        } 
      //lastPublishMillis = - pubInterval;
    }

// USER CODE EXAMPLE
} 

void message(char* topic, byte* payload, unsigned int payloadLength) {
    byte2buff(msgBuffer, payload, payloadLength);
    StaticJsonDocument<512> root;
    DeserializationError error = deserializeJson(root, String(msgBuffer));
  
    if (error) {
        Serial.println("handleCommand: payload parse FAILED");
        return;
    }

    handleIOTCommand(topic, &root);
    if (!strcmp(updateTopic, topic)) {
        // user variable update
    } else if (!strncmp(commandTopic, topic, cmdBaseLen)) {            // strcmp return 0 if both string matches
        handleUserCommand(&root);
    }
}

void setup() {
   // put your setup code here, to run once:
    Serial.begin(115200);

    initDevice();
    // If not configured it'll be configured and rebooted in the initDevice(),
    // If configured, initDevice will set the proper setting to cfg variable

    WiFi.mode(WIFI_STA);
    WiFi.begin((const char*)cfg["ssid"], (const char*)cfg["w_pw"]);
    while (WiFi.status() != WL_CONNECTED) {
        delay(500);
        Serial.print(".");
    }
    // main setup
    Serial.printf("\nIP address : "); Serial.println(WiFi.localIP());

    set_iot_server();
    client.setCallback(message);
    iot_connect();

    servo.attach(4);
    //initial servo setting
    servo.write(0);
    publishData();

}

void loop() {
    if (!client.connected()) {
        iot_connect();
    }
// USER CODE EXAMPLE : main loop
//     you can put any main code here, for example, 
//     the continous data acquistion and local intelligence can be placed here
// USER CODE EXAMPLE
    client.loop();
}