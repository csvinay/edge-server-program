import paho.mqtt.client as mqtt
import time
import csv
import json
import datetime
# This is the Publisher

def MQTT_publish(broker, file, topic):
    def on_publish(client,userdata,result):             #create function for callback
        print("data published \n")
        pass
    client = mqtt.Client(client_id="1259396903", clean_session=False)
    print("Connecting to broker", broker)
    client.connect(broker)
    #client.loop_start()
    print("Publishing...")
    client.loop_start()
    myfile = open(file, 'r')
    reader = csv.reader(myfile)
    next(reader)
    mylist = list(reader)
    myfile.close()
    for line in mylist:
        ts = time.time()
        tst = datetime.datetime.fromtimestamp(ts).strftime('%Y-%m-%d %H:%M:%S')
        line.append(tst)
        strline = json.dumps(line)
        client.on_publish = on_publish  
        client.publish(f"{topic}", strline, qos=2, retain=True)
        time.sleep(1)
    client.loop_stop()

def on_message(client, userdata, message):
    print(str(message.payload.decode("utf-8")))


def MQTT_subscribe(broker, topic):
    client = mqtt.Client()
    print("Connecting to broker", broker)
    client.connect(broker)
    print(f"Subscribing {topic}")
    client.subscribe(f"{topic}")
    client.on_message = on_message
    client.loop_forever()

broker = "mqtt.eclipseprojects.io"
file = "dataset.csv"
topic="datarow"
MQTT_publish(broker, file, topic)

