# This is the Subscriber. The publsihed data is subscribed from the broker through the specified topic

import paho.mqtt.client as mqtt
import time
import csv
from csv import writer
import json
import datetime

def on_message(client, userdata, message):
    #Storing the datarow subscribed as string
    strrow = str(message.payload.decode("utf-8"))
    #Converting string to list
    row = json.loads(strrow)
    #Adding subscribed time to the data row
    ts = time.time()
    tst = datetime.datetime.fromtimestamp(ts).strftime('%Y-%m-%d %H:%M:%S')
    row.append(tst)
    print("Receieved data successfully",row)
    #Appending the datarow to the recvdataset.csv
    with open('recvdataset.csv', 'a') as f_object:
        writer_object = writer(f_object)
        writer_object.writerow(row)
        f_object.close()

#Function to connect to the broker and subscribe the datarows
def MQTT_subscribe(broker, topic):
    #Unique client_id is given so that even if the process is broken and started again, 
    # The previous published datarows can be retained and subscribed
    client = mqtt.Client(client_id="1259396904", clean_session=False)
    print("Connecting to broker", broker)
    client.connect(broker)
    print(f"Subscribing {topic}")
    client.subscribe(f"{topic}", qos=2)
    client.on_message = on_message
    client.loop_forever()

broker = "mqtt.eclipseprojects.io"
topic="datarow"
MQTT_subscribe(broker,topic)