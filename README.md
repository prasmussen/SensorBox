SensorBox
=========


## Overview
SensorBox is a hardware device used to log various data in my home. It consists of an Arduino microcontroller
and currently has sensors to log motion, temperature and humidity. The Arduino logs motion events and 
temperature/humidity changes directly to a CouchDB as JSON data.


## Parts list
- [Arduino Ethernet with PoE module](http://store.arduino.cc/ww/index.php?main_page=product_info&cPath=11_12&products_id=142)
- [PIR (motion) sensor](http://adafruit.com/products/189)
- [AM2302 (wired DHT22) temperature-humidity sensor](http://adafruit.com/products/393)


## Example data from CouchDB
###### Movement event
    {
       "_id": "a631c192ffebb7b0d543863925f3a53a",
       "_rev": "1-f84d014d548f1767bd3cc79887e39f7c",
       "source": "sensorbox",
       "location": "living room",
       "type": "movement",
       "value": false,
       "created": 1358266605704
    }

###### Humidity
    {
       "_id": "a631c192ffebb7b0d543863925f65f4c",
       "_rev": "1-c010833fb06a57f7415cbff3d2fb2645",
       "source": "sensorbox",
       "location": "living room",
       "type": "humidity",
       "value": 20.7,
       "created": 1358278790983
    }

###### Temperature
    {
       "_id": "a631c192ffebb7b0d543863925f4e8f9",
       "_rev": "1-9e8bacc2a3b79a2dc37ffeb5c53383f9",
       "source": "sensorbox",
       "location": "living room",
       "type": "temperature",
       "value": 25.3,
       "created": 1358270795751
    }
