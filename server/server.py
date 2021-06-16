import paho.mqtt.client as mqtt
import time
import csv
from csv import writer
import json
import datetime
# This is the Subscriber

def on_message(client, userdata, message):
    #print(str(message.payload.decode("utf-8")))
    strrow = str(message.payload.decode("utf-8"))
    row = json.loads(strrow)
    ts = time.time()
    tst = datetime.datetime.fromtimestamp(ts).strftime('%Y-%m-%d %H:%M:%S')
    row.append(tst)
    print("Receieved data successfully",row)
    with open('recvdataset.csv', 'a') as f_object:
        writer_object = writer(f_object)
        writer_object.writerow(row)
        f_object.close()

def MQTT_subscribe(broker, topic):
    client = mqtt.Client(client_id="1259396904", clean_session=False)
    print("Connecting to broker", broker)
    client.connect(broker)
    print(f"Subscribing {topic}")
    client.subscribe(f"{topic}")
    client.on_message = on_message
    client.loop_forever()

broker = "mqtt.eclipseprojects.io"
topic="datarow"
MQTT_subscribe(broker,topic)