<!-- ABOUT THE PROJECT -->
# Zentaix IoT Intern Task

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
** Example: 1st & 2nd minute, 1st & 2nd data points published successfully, 3rd & 4th minute, 3rd & 4th data points failed and got buffered, 5th min 5th datapoint and all the buffered data published.

Final Output = dataset.csv file is read and all the points are published to the server.
