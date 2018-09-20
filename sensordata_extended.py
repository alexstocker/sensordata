#!/usr/bin/python
import sys
import time
import Adafruit_DHT
import threading
import json
import urllib2
import base64
import pprint
from datetime import datetime

start_time = time.time()
interval = 10 # set interval in seconds
deviceId = 'SOMEUNIQUE-DEVICE-ID' # for example: a uuid like this f7645058-fe8c-11e6-bc64-92361f002671
url = 'https://SOME.URL/TO/POST/TO' # for example: https://YOUR-OWNCLOUD-INSTANCE/index.php/apps/sensorlogger/api/v1/createlog/ if you are using SensorLogger for owncloud
headers = {'content-type': 'application/json'}

sensor_args = { '11': Adafruit_DHT.DHT11,
                '22': Adafruit_DHT.DHT22,
                '2302': Adafruit_DHT.AM2302 }
if len(sys.argv) == 3 and sys.argv[1] in sensor_args:
    sensor = sensor_args[sys.argv[1]]
    pin = sys.argv[2]
else:
    print('usage: sudo ./Adafruit_DHT.py [11|22|2302] GPIOpin#')
    print('example: sudo ./Adafruit_DHT.py 2302 4 - Read from an AM2302 connected to GPIO #4')
    sys.exit(1)

def sensorData():
    threading.Timer(interval, sensorData).start()
    humidity, temperature = Adafruit_DHT.read_retry(sensor, pin)
    cdatetime = datetime.now()
    currentDate = cdatetime.strftime('%Y-%m-%d %H:%M:%S')

    if humidity is not None and temperature is not None:
#Example of payload for extended data see : https://github.com/alexstocker/sensorlogger/blob/master/tests/curl/post_extend_0.php
    payload = {
   'deviceId': deviceId,
   'date': currentDate,
   'data': [{'dataTypeId':'1',
            'value' : temperature},
            {'dataTypeId':'2',
             'value' : humidity},
            {'dataTypeId':'4',
             'value' : AIQ}]
  }


        req = urllib2.Request(url)

		base64string = base64.encodestring('%s:%s' % ('SOMEUSERNAE', 'SOMEPASSWORD')).replace('\n', '')
		req.add_header("Authorization", "Basic %s" % base64string)
		req.add_header("Content-Security-Policy", "default-src 'none';script-src 'self' 'unsafe-eval';style-src 'self' 'unsafe-inline';img-src 'self' data: blob:;font-src 'self';connect-src 'self';media-src 'self'")
        req.add_header('Content-Type','application/json')
        data = json.dumps(payload)
		response = urllib2.urlopen(req,data)
	else:
        print('Failed to get reading. Try again!')
        sys.exit(1)

sensorData()
