<!-- ABOUT THE PROJECT -->
# Zentaix IoT Intern Task

Consider that an IoT device publishes data to a middleware software which is then published to the cloud.

While you are developing an IoT solution, you do not have access to all the physical sensors and therefore you decide to simulate all types of sensors that exist in a candy factory. A CSV is shared which contains simulated sensor data.

In this project, we have two programs:
* **Edge Program**: 
  
  A executable python program to read sensor data and send it to the cloud server.
  
* **Server Program**: 
  
  A simple python MQTT broker to accept data from edge program and save it in a CSV file.

## Built With

### Operating System 
* Ubuntu 20.04.2 LTS (16GiB Memory)

### Programming Language
* Python 3.8.8

### Libraries
* paho-mqtt 1.5.1 
* time
* csv
* json 
* datetime

<!-- GETTING STARTED -->
## Getting Started

To get a local copy up and running follow these simple steps.

### Prerequisites

0. Setup the conda environment and download the required dependencies. (or else it can even be done on the existing python3 setup on your local machine according to your convenience)

* Open terminal.
* run the following command in the terminal (Incase anaconda is not installed on your system - Do follow [this documentation](https://docs.anaconda.com/anaconda/install/)):
  ```sh
   conda create -n zenatixtask python=3.8.8
   ```
   
   activate the environment using the command:
   ```sh
   conda activate zenatixtask
   ```
* run the follwing command to download the dependencies 
  ```sh
  pip install -r requirements.txt
  ```  

### Server Program

1. The following tasks are accomplished by running the server program:

* The server program acts as a subsriber to receive data from edge program.
* A route on a topic on MQTT broker accepts data. 
* It gives success or failure for the received data. 
* Each new data is appended to a CSV file.

Output = recvdataset.csv file is generated with all the data points received from the dataset.csv through edge program. 

### Edge Program

2. The following tasks are accomplished by running the edge program:

* Each data point is read after every 60-sec delay and publish to the cloud(live data)
* If the broker(server) returns failure or server is stopped the data point is buffered locally(it now becomes buffered data).
* Once the connection is restablished by the server program, All the buffered data will be published along with the live data without any disturbance. While publishing and cleaning the buffered data, the live data is not stopped (which is obtained by the mqtt functionalities)
- Example: 1st & 2nd minute, 1st & 2nd data points published successfully, 3rd & 4th minute, 3rd & 4th data points failed and got buffered, 5th min 5th datapoint and all the buffered data published.
* A function to get the count of successfully transmitted and buffered data at any point in time.

Final Output = dataset.csv file is read and all the points are published to the server.

## Run the Projcet

First we have to run the `server.py` program so that it connects to the external broker and can wait for the edge program to publish the data. 

Run the follwing two commands to run the server.py program
  ```sh
  cd server
  python server.py
  ```  
Now the server connects to the broker and subscribes the data points from the particular topic published by the edge program.

To run the `edge.py`, run the following commands from initial project directory.
  ```sh
  cd edge
  python edge.py
  ```  
  Here the `edge.py` program connects to the external broker and publishes the data on the specified topic and also returns whether the data has been published or not. Even if the edge program is not connected with the broker, the data rows get buffered and when it is connected with the broker they get received by the broker and will be subscribed by server.py program. 

## Output

Firstly, after running the `server.py` program in one terminal, we get the following output:
![image](https://user-images.githubusercontent.com/43710239/122220987-7b2c0780-cece-11eb-8c79-1bd5897d3cb0.png)

Then the server.py program waits for the edge.py program to publish data so that it can receive the data. 

Then, we will run the `edge.py` program in another terminal, and we get the folliwng initial output:
![image](https://user-images.githubusercontent.com/43710239/122267703-d7a41c80-cef8-11eb-88b3-65a8d2dffcca.png)

Then the data is transfered from edge.py program side to server.py program side and we can see the data received successfully log on the server.py terminal as follows:

![output](https://user-images.githubusercontent.com/43710239/122281181-9cf5b080-cf07-11eb-8366-52de00f92836.png)

The above output is produced when the server.py program is run first and then the edge.py program is run next and there are no disturbances in the server and edge side. 

To test the reliability of the programs, several test cases were run where there were random disturbances in the server and edge side. 

NOTE: For faster testing purpose, the time interval between each data row transfer is changed from 60 seconds to 5 seconds (This can be altered in the edge.py program in line 37

### Test case 1: Edge side connection is lost/broken and server side is running

In this case, the edge side is broken and the server side would be running smoothly. To test this case, 

First, the edge.py and the server.py are run one after another and then, 

The connection of edge.py is broken by turning off the wireless connectivity to the edge.py program side terminal.

After sometime the wifi is turned on and the connection is re-established, the output is as follows:

![image](https://user-images.githubusercontent.com/43710239/122283628-61a8b100-cf0a-11eb-9dd4-cbcb752f74d6.png)

Here we can see that till 3rd datarow transfer, the connection was good and there was no distubance. After the 3rd data row transfer, the connection is cut (wifi was turned off) and after sometime (around 12-15 seconds), the wifi was turned on again and the connection was re-established, and here we can see that `4,5,6` rows were buffered locally and queued upto the latest data row (6th) and then published immediately after the connection is established. 

![output(1)](https://user-images.githubusercontent.com/43710239/122284304-13e07880-cf0b-11eb-8212-664469a0122d.png)

Here in the above image, we can see the log of the data received by the server.py program. Till 3rd data row everything was fine and after that, we can observe that the datarows once published by the edge.py after the break were immediately retained and subscribed by the server.py program from the local buffer of the edge.py program. The timestamp marked in green rectangle depicts the datarows subscribed time and shows us  that the data rows have been immediately subscribed after the edge.py program has pubished the queue after the re-establishment of the connection. The column of the blue rectangle specifies the time the datarow has been published/queued on the edge.py program side.   

### Test case 2: Server side connection is broken and edge side is running


In this case, the server side is broken and the edge side would be running smoothly. To test this case, 

First, the edge.py and the server.py are run one after another and then, 

The connection of server.py is broken by turning off the wireless connectivity to the server.py program side terminal.

After sometime the wifi is turned on and the connection is re-established, the output is as follows:

![image](https://user-images.githubusercontent.com/43710239/122285158-0081dd00-cf0c-11eb-8500-6b67aa583ee2.png)

Here we can see that the data rows are being publsihed from the edge.py program smoothly. 

![output(2)](https://user-images.githubusercontent.com/43710239/122285108-efd16700-cf0b-11eb-8086-41dbda44f7b6.png)

Here in the above image, we can see the log of the data received by the server.py program. Till 3rd data row everything was fine and after that, we can observe that the server.py connection is cut by turning off the wifi and once the wifi is turned on and the connection is re-established by the server.py program, the datarows published by the edge.py were immediately retained and subscribed by the server.py program from the broker. The timestamp marked in green rectangle depicts the datarows subscribed time and shows us that the data rows have been immediately subscribed after the server.py program has re-established the connection. The column of the blue rectangle specifies the time the datarow has been published/queued on the edge.py program side. 

### Test case 3: Server side program is stopped and rerun and edge side is running


In this case, the server side program is broken and stopped in between and the edge side would be running smoothly. To test this case, 

First, the edge.py and the server.py are run one after another and then, 

The server.py is broken by stopping it using `ctrl+c`

After sometime the server.py program is rerun and, the output is as follows:

![image](https://user-images.githubusercontent.com/43710239/122285158-0081dd00-cf0c-11eb-8500-6b67aa583ee2.png)

Here we can see that the data rows are being publsihed from the edge.py program smoothly. 

![output(3)](https://user-images.githubusercontent.com/43710239/122285583-8d2c9b00-cf0c-11eb-987f-3af91821f376.png)

Here in the above image, we can see the log of the data received by the server.py program. After receiving the first two rows, the program was forced to stop and then we can observe that the server.py connection is broken and once the the program is rerun and the connection is re-established by the server.py program, the datarows published by the edge.py were immediately retained and subscribed by the server.py program from the broker. The timestamp marked in green rectangle depicts the datarows subscribed time and shows us that the data rows have been immediately subscribed after the server.py program has re-established the connection. The column of the blue rectangle specifies the time the datarow has been published/queued on the edge.py program side. 

### Test case 4: Edge side program is first run and then server side program is running



### Test case 5: First the Edge side program is run and then after sometime the server side program is run




