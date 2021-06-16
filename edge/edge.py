import paho.mqtt.client as mqtt
import time
import csv
import json
import datetime

def MQTT_publish(broker, file, topic):
    def on_publish(client,userdata,result):             #create function for callback
        print("data row:",count,"published")
        pass
    count = 0 
    client = mqtt.Client()
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
        count +=1
        client.publish(f"{topic}", strline, qos=2, retain=True)
        #counting the number of data rows published
        client.on_publish = on_publish
        time.sleep(1) 
    client.loop_stop()

broker = "mqtt.eclipseprojects.io"
file = "dataset.csv"
topic="datarow"
MQTT_publish(broker, file, topic)

