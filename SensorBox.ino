#include <SPI.h>
#include <Ethernet.h>
#include "DHT.h"


// Pins
#define DHT_PIN 2
#define DHT_TYPE DHT22
#define PIR_PIN 9

// Mac address
byte mac[] = {0x90, 0xA2, 0xDA, 0x00, 0x76, 0x64};

// Http client config
EthernetClient client;
#define PATH "/sandvikbakken/_design/app/_update/event"
#define HOST "couchdb.host.cx"
#define PORT 5984

// Base64 user:password
#define AUTH_BASIC "YXV0bzphdXRv"

// JSON helpers
#define SOURCE "\"sensorbox\""
#define LOCATION "\"living room\""
#define TYPE_MOVEMENT "\"movement\""
#define TYPE_TEMPERATURE "\"temperature\""
#define TYPE_HUMIDITY "\"humidity\""
#define JSON_FORMAT(t0, t1) "{\"source\":" SOURCE ",\"location\":" LOCATION ",\"type\":" t0 ",\"value\":" t1 "}"

// Global state
#define BUFFER_SIZE 90
#define RECENT_LENGTH 3
int motion_previous = -1;
float temperature_recent[RECENT_LENGTH];
float humidity_recent[RECENT_LENGTH];
char buffer[BUFFER_SIZE];
DHT dht(DHT_PIN, DHT_TYPE);


void wait_for_dhcp() {
  while (1) {
    if (Ethernet.begin(mac)) {
      return;
    }
    delay(500);
  }
}

void setup() {
  //Serial.begin(9600);
  pinMode(PIR_PIN, INPUT);

  wait_for_dhcp();

  // Init temperature and humidity sensor
  dht.begin();
}

int occured_recently(float *recent, float value) {
  for (int i = 0; i < RECENT_LENGTH; i++) {
    if (recent[i] == value) {
      return 1;
    }
  }	
  return 0;
}

void add_recently(float *recent, float value) {
  // Shift all elements in the array to the left, this will overwrite the first element
  int elem_size = sizeof(recent[0]);
  int bytes_to_move = (RECENT_LENGTH * elem_size) - elem_size;
  memmove(&recent[0], &recent[1], bytes_to_move);

  // Add value as the last element of the array
  recent[RECENT_LENGTH - 1] = value;
}


void post_data() {
  if (!client.connect(HOST, PORT)) {
    client.stop();
    return;
  } 
  
  char content_length[22];
  sprintf(content_length, "Content-Length: %d", strlen(buffer));
  
  client.println("POST " PATH " HTTP/1.1");
  client.println("Host: " HOST);
  client.println("User-Agent: arduino");
  client.println("Connection: close");
  client.println(content_length);
  client.println("Authorization: Basic " AUTH_BASIC);
  client.println();
  client.println(buffer);
  client.stop();
}

void post_motion_event(int movement) {
  memset(buffer, '\0', BUFFER_SIZE);

  if (movement) {
    sprintf(buffer, JSON_FORMAT("%s", "%s"), TYPE_MOVEMENT, "true");
  } 
  else {
    sprintf(buffer, JSON_FORMAT("%s", "%s"), TYPE_MOVEMENT, "false");
  }

  post_data();
}

void post_temperature_event(float value) {
  memset(buffer, '\0', BUFFER_SIZE);

  // Convert float to string
  char celcius[8];
  dtostrf(value, 1, 2, celcius);
  
  sprintf(buffer, JSON_FORMAT("%s", "%s"), TYPE_TEMPERATURE, celcius);
  post_data();
}

void post_humidity_event(float value) {
  memset(buffer, '\0', BUFFER_SIZE);
  
  // Convert float to string
  char percent[8];
  dtostrf(value, 1, 2, percent);
  
  sprintf(buffer, JSON_FORMAT("%s", "%s"), TYPE_HUMIDITY, percent);
  post_data();
}


void loop() {
  // Read motion sensor
  int motion = digitalRead(PIR_PIN);
  if (motion != motion_previous) {    
    motion_previous = motion;
    post_motion_event(motion);
  }

  // Read temperature
  float temperature = dht.readTemperature();
  if (!isnan(temperature) && !occured_recently(temperature_recent, temperature)) {
    add_recently(temperature_recent, temperature);
    post_temperature_event(temperature);
  }

  // Read humdity
  float humidity = dht.readHumidity();
  if (!isnan(humidity) && !occured_recently(humidity_recent, humidity)) {
    add_recently(humidity_recent, humidity);
    post_humidity_event(humidity);
  }
}

