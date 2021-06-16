# This is the publisher. The data is published to the broker through a specific topic

import paho.mqtt.client as mqtt
import time
import csv
import json
import datetime
import threading

broker = "mqtt.eclipseprojects.io"
file = "dataset.csv"
topic="datarow"

# To count the number of data rows transfere
count = 0 
#create function for callback whether the data is published or not
def on_publish(client,userdata,result):             
    print("data row:",count,"published")
    pass

#Creating a client instance
client = mqtt.Client()

#Connecting to the broker
print("Connecting to broker", broker)
client.connect(broker)

#Function to publish the datarow to the server
def publishing():
    print("Publishing...")
    client.loop_start()
    #Reading the dataset.csv file
    myfile = open(file, 'r')
    reader = csv.reader(myfile)
    next(reader) #Skipping the header
    #Reading all the rows as list
    mylist = list(reader)
    myfile.close()
    #Loop to read each row individually and publish it to the broker
    for line in mylist:
        global count
        #Adding publishing time to the list
        ts = time.time()
        tst = datetime.datetime.fromtimestamp(ts).strftime('%Y-%m-%d %H:%M:%S')
        line.append(tst)
        strline = json.dumps(line)

        #counting the number of data rows published
        count +=1
        
        #Publishing the list
        client.publish(f"{topic}", strline, qos=2, retain=True)
        
        #Giving an interval of 60 seconds between datarows transfer
        time.sleep(5) 
    client.loop_stop()

#create function for callback whether the data is published or not
def counting():
    client.on_publish = on_publish

#Multithreading to publish the data and count the datarows transfered simultaneously without any disturbance
threading.Thread(target=counting()).start()
threading.Thread(target=publishing()).start()