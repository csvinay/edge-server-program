/*
To run the client side code:
1. First run the server side code by doing the following steps:
- open terminal in the server directory.
- run the commnad "g++ server.cpp -o server"
- run the command "./server 9988" (instead of 9988 any number can be entered)
2. Now open terminal in the client directory
3. Run the commnad "g++ client.cpp -o client"
4. Run the command "./server 9988" (use the same number as used while running server.)
GOOD TO GO!
*/
#include <iostream>
#include <sys/socket.h>
#include <stdlib.h>
#include <netinet/in.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <sstream>
#include <unistd.h>
#include <string.h>
#include <fstream>

using namespace std;

string get_data_string(string file_name) // converts file contents into a string
{
	ifstream file(file_name.c_str());
    string word;
    char x ;
    word.clear();
    int count  = 0;

    while ((x = file.get())!= EOF)
    {
        word = word + x;
    }
    file.close();
    return word;
}

void str2char(string data, char* destination){  //converts string into char array
	int size = data.size();
	char strarray[size];
	for(int i = 0;i < size;i++){
		destination[i] = (char)data[i];
	}	
}

int send(int file_descriptor,string message,int count){ //send data to given fle descriptor of UNIX
	char buffer[2*count];	
	bzero(buffer,2*count);
	str2char(message,buffer);
	int n = -1;	
	n = write(file_descriptor,buffer,count);
	if(n<0){
		cout<<"\nError while writing to the port!\n"<<endl;
		}
	return n;
}

string recieve(int file_descriptor, int count){ //returns string read on given file descriptor
	char buffer[2*count];
	bzero(buffer,2*count);
	int n = read(file_descriptor,buffer,count);
	if(n<0){
		cout<<"\nError while reading from the socket!\n"<<endl;
	}	
	string stri(buffer);
	return stri;
}

void show_available_files(int file_descriptor){ //calls ls system call internally and sens list of iles available in current directory
	int n = send(file_descriptor,"ls",128);
	if(n<0){
		cout<<"\n Error while asking to show files!\n"<<endl;
	}
	string list = recieve(file_descriptor,512);
	if(list.size()<1){
		cout<<"\n Error while reading list of files!\n"<<endl;		
	}
	else{
		cout<<list<<endl;
	}
}

void put_file(int file_descriptor, string file_name){ //puts a new file uploaded by client on server
	string content = get_data_string(file_name);
	int n = send(file_descriptor,string("put")+" "+file_name,256);
	if(n<0){
		cout<<"\nError while sending the file!\n"<<endl;
	}
	n = send(file_descriptor,content,1024);
		
	if(n<0){
		cout<<"\nError while uploading the file!\n"<<endl;
	}	
}

void get_file(int file_descriptor,string file_name){ //called by client to download from server
	int n = send(file_descriptor,string("get")+" "+file_name,256);
	string content = "";
	if(n<0){
		cout<<"\nError while requesting the file!\n"<<endl;
	}
	else{				
	  	content = recieve(file_descriptor,1024);
	  	if(content.size()<1){
	  		cout<<"\nError while downloading the file!\n"<<endl;
	  	}
	  	else if(content == "\nThere are no such files found in the directory!\n"){
	  		cout<<content<<endl;
	  		cout<<"\nPlease check the files available in the directory using command ls.\n"<<endl;
	  	}
	  	else{
	  		ofstream file;
	  		file.open(file_name.c_str());
	  		file << content;
  			file.close(); 
  			cout<<"\nFile Downloaded succesfully!\n"<<endl;
	  	}  		
	}
}

void show_menu(int file_descriptor){ // a menu driven program function to help user interct with FTP	
	int loop_invariant = 0;
	int choice = -1;	
	string file_name = "";
	while(loop_invariant == 0){
		cout<<"\nEnter command to\n"<<endl<<"1. get list of files\n"<<endl<<"2. download file\n"
		 <<endl<<"3. upload file\n" <<endl<<"4. get current time\n"<<endl<<"5. get current directory\n"<<endl<<"6. create new file\n"<<endl
		 <<"7. delete file\n"<<endl<<"8. exit\n"<<endl;
		cin>>choice;
		switch(choice){
			case 1: show_available_files(file_descriptor);
					break;
			case 2:cout<<"\nPlease enter the file name with extension also to download.\n"<<endl;
					cin>>file_name;
					get_file(file_descriptor,file_name);
					break;
			case 3:cout<<"\nPlease enter the file name with extension also to upload.\n"<<endl;
					file_name.clear();
					cin>>file_name;
					put_file(file_descriptor,file_name);
					break;
			case 4: cout<<"\nCurrent time at server is: "<<endl;
					send(file_descriptor,string("time"),256);
					cout<<recieve(file_descriptor,25);
					break;
			case 5: cout<<"\nCurrent working directory is: "<<endl;
					send(file_descriptor,string("whereami"),256);
					cout<<recieve(file_descriptor,25);
					break;
			case 6: cout<<"\nPlease enter the file name with extension to create the file!\n"<<endl;
					cin>>file_name;
					send(file_descriptor,string("touch")+" "+file_name,256);
					break;
			case 7: cout<<"\nPlease enter the file name along with extension to delete it.\n"<<endl;
					cin>>file_name;
					send(file_descriptor,string("Delete")+" "+file_name,256);
					break;
			case 8: loop_invariant = 1;
					break;
		}
	}
}


int main(){
	int domain = AF_INET;
	int type = SOCK_STREAM;
	int protocol = 0;
	int status = 0;
	int file_descriptor = 0;
	int port_number = 5010;	
	struct sockaddr_in server_socket;   	

	file_descriptor = socket(domain,type,protocol);
	if(file_descriptor < 0){
		cout<<"\nERROR: Failed to create sockets!\n"<<endl;
	}
	else{		
   			bzero((char *) &server_socket, sizeof(server_socket));
   			server_socket.sin_family = AF_INET;   			
   			server_socket.sin_addr.s_addr = inet_addr("127.1.0.1");
   			server_socket.sin_port = htons(port_number);   			
   			status = connect(file_descriptor, (struct sockaddr*)&server_socket, sizeof(server_socket));
   			if(status < 0){
   				cout<<"\nERROR: Could not connect, please try again!\n"<<endl; 
   			}
   			else{
   				cout<<"\nCONNECTED SUCCESFULLY!\n"<<endl;   				
   				show_menu(file_descriptor);
   			}
	}
}