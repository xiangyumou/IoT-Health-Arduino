#include "LinkedList.h"
// #include <SoftwareSerial.h>

#include "DHTSensor.h"
#include "SoundSensor.h"
#include "Light.h"
#include "Button.h"

#define DEVICE_CODE "b5ab92178e4b404b9f83a98abe2d7ce3"

#define LOOP_INTERVAL 300
#define PORT 115200

#define DHT11_PIN 2
#define SOUND_SENSOR_PIN 3
#define LIGHT_PIN 4
#define BUTTON_CANCEL_PIN 5
#define BUTTON_ACTIVATE_PIN 6
#define RX 7
#define TX 9


DHTSensor dht(DHT11_PIN, DHT11);
SoundSensor sds(SOUND_SENSOR_PIN, 30);
Light lgt(LIGHT_PIN, 1);
Button cel_btn(BUTTON_CANCEL_PIN);
Button act_btn(BUTTON_ACTIVATE_PIN);

// SoftwareSerial wifi_serial = SoftwareSerial(TX, RX);

unsigned int CUR_LOOP;

void setup() {
  Serial.begin(PORT);
  while (!Serial) delay(10);
  Serial2.begin(9600);
  while (!Serial2) delay(10);
  // wifi_serial.begin(9600);
  // wifi_serial.flush();

  CUR_LOOP = 0;

  dht.begin();
  sds.begin();
  lgt.begin();
  cel_btn.begin();
  act_btn.begin();
}

String get_json(LinkedList<String> &data_list) {
  String res = "{";
  int n = data_list.size();
  for (int i = 0; i < n; i++) {
    res += data_list[i] + " }"[i == n - 1];
  }
  return res;
}

String get_device_id() {
  String code = DEVICE_CODE;
  code = "\"deviceCode\": \"" + code + "\",";
  return code;
}

void loop() {
  delay(10);
  dht.update();
  sds.update();

  int button_state = cel_btn.read();
  if (button_state) {
    lgt.close();
  }
  button_state = act_btn.read();
  if (button_state) {
    lgt.open();
  }

  if (CUR_LOOP % 100 == 0) {
    Serial.println(CUR_LOOP);
  }

  if (++CUR_LOOP == LOOP_INTERVAL) {
    CUR_LOOP = 0;
    LinkedList<String> ls;
    ls.add(sds.get());
    ls.add(dht.get());
    ls.add(get_device_id());
    ls.add(lgt.get_state_json("alert"));
    String res = get_json(ls);
    ls.clear();
    // Serial.println(res);
    Serial2.println(res);
  }

  if (Serial2.available()) {
    String message = Serial2.readString();
    Serial.print("received: [" + message + "]");
    if (message == "0") {
      lgt.close();
    } else {
      lgt.open();
    }
  }
  // wifi_serial.println("Hello from Arduino");
  // // Serial.println("running");
  // Serial.println(wifi_serial.available());

  // delay(2000);
}
