/*
As per the given assignment, the following are the requirements:

[I] Client Model should have following functions
1) Socket
2) Connect
3) Write
4) Read
5) close

[II] Send and receive a TEXT from Cleint machine and receive from the Server machine

[III] sending files and receiving files such as text/images/audio/video. 

All the above requirements have been fulfilled and below begins the code of the client.c 
*/


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <netdb.h>
#include <ctype.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <arpa/inet.h>

//Error function to exit the program
void error(const char *msg)
{
	perror(msg); /*perror is an inbuilt function that interprets the error no. and outputs the output error description by using STDERR */
	exit(1);
}

int main(int argc, char *argv[])
{
	/*
	The user need to provide 3 arguments, 
	(1)the file name - argv[0]
	(2)the ip address of server - argv[1]
	(3)the port no. - argv[2]
	*/

	int socketfd, portno, n; //socketfd = file descriptor

	struct sockaddr_in serv_address; //sockaddr_in = provides internet address

	struct hostent * server; //hostent structure = stores info about the host.
	int sourse_fd;

	char buffer[1024]; //buffer will be transferred in a data stream

	int opr, sub_opr; //declaring variables for operation and sub operations which will be defined later. 

	char str[20]; //declaring file name 
	int file_name_len, read_len;

	if (argc < 3) //checking whether all the inputs are given or not	
	{
		fprintf(stderr, "usage %s hostname port\n", argv[0]);
		exit(0); //exits if all the three inputs are satisfied
	}

	
	portno = atoi(argv[2]); //converting the string into int variable type

	/*
	socket function has 3 arguments, 
	(1)domain, here we will use AF_INET which is an IPv4 protocol,
	(2)type, we will use SOCK_STREAM as it for TCP protocol.
	(3)protocol, which is 0 for TCP 
	*/
	socketfd = socket(AF_INET, SOCK_STREAM, 0);

	if (socketfd < 0)
	{
		error("There is an error opening the socket");
	}

	server = gethostbyname(argv[1]); //receiving information about the host
	if (server == NULL)
	{
		fprintf(stderr, "host doesn't exists");//There won't be host if server does not exist
	}

	//bzero clears data or anything in whatever it is refrenced to
	//clearing serv_address so as to do operations on it 
	bzero((char*) &serv_address, sizeof(serv_address));

	/*
	htons - host to network short (to get port, using info),
	*/
	serv_address.sin_family = AF_INET;
	bcopy((char*) server->h_addr, (char*) &serv_address.sin_addr.s_addr, server->h_length);
	serv_address.sin_port = htons(portno);

	//conneting to host, if file descriptor is less than 0, then the operation failed
	if (connect(socketfd, (struct sockaddr *) &serv_address, sizeof(serv_address)) < 0)
		error("Could not connect");

	fflush(stdout); //clearing everything in the output buffer of a stream

	//Asking user to select an option to perform it
	printf("What operation has to be performed?\n Please select a number, for example: 1 \n1) Send File\n2) Chat with server \nPress other key to exit\n ");
	//taking user input
	scanf("%d", &opr);

	//the selected operation is sent to the server for further process
	int number_to_send;
	fflush(stdout);
	number_to_send = opr;
	int converted_number = htonl(number_to_send);

	// Informing the server about the operation using write
	write(socketfd, &converted_number, sizeof(converted_number));

	//Operation execution
	switch (opr)
	{
		case 1:
			//user selected send file
			{
			 	//asking user to select file type
				bzero(str, 20);
				printf("Please select type of file. \n1) Text file \n2) Image \n3) Audio \n4) Video \nPress any other key to terminate the program\n");

				//Sending this sub operation to server
				scanf("%d", &sub_opr);
				int sub_number_to_send;

				//Telling the server about the sub - operation
				fflush(stdout);
				sub_number_to_send = sub_opr;
				int sub_converted_number = htonl(sub_number_to_send);
				write(socketfd, &sub_converted_number, sizeof(sub_converted_number));

				//Execution of sub operation	
				switch (sub_opr)
				{
					case 1:
						{
							printf("Enter the name of the file you want to send along with the extension, Example: sample.txt\nNote: the file should be in the same directory as the client.c file\n");

							//clearing everything in str, so as to save the file name in str
							//we will send this file name to server
							bzero(str, 20);
							scanf("%s", str);
							n = write(socketfd, str, strlen(str));

							//Starting the operation to send the txt file
							FILE * f;

							int words = 0;
							char c;
							f = fopen(str, "r");
							while ((c = getc(f)) != EOF)
							//Counting No of words in the file
							{
								fscanf(f, "%s", buffer);
								if (isspace(c) || c == '\t')
									words++;
							}

							write(socketfd, &words, sizeof(int));
							rewind(f);
							char ch;
							/*
							On getting EOF (or error) from standard input, the client Stdin process, before it exits, needs to tell
							the server that the client is done and there is nothing more coming down
							the socket.  The client has to signal "EOF" to the server.
							*/
							//So we will keep sending the file through buffer until EOF is encountered
							while (ch != EOF)
							{

								fscanf(f, "%s", buffer);
								//For printing every word in the file
								//printf("%s\n", buffer);	
								write(socketfd, buffer, 512);
								ch = fgetc(f);
							}
							//The operation executed successfully
							//The file was sent successfully
							printf("The file was sent successfully\n");
							break;
						}
					case 2:

						{

							/*memset copies the character 0 (an unsigned char) to the first 1024 characters of the string pointed to, by the argument buffer*/
							memset(buffer, 0x00, 1024);
							printf("Enter the name of image that you want to send along with the extension, Example: image.png\nNote: the file should be in the same directory as the client.c file\n");
							scanf("%s", buffer);
							file_name_len = strlen(buffer);

							//sending file name to server
							send(socketfd, buffer, file_name_len, 0);

							//opening the file in the buffer for reading
							sourse_fd = open(buffer, O_RDONLY);
							if (!sourse_fd)
							{
								perror("Problem in reading the file");
								return 1;
							}

							while (1)
							{

								//reading the file and then sending it through buffer, if read_len is 0 then there is nothing to send and the loop gets terminated
								memset(buffer, 0x00, 1024);
								read_len = read(sourse_fd, buffer, 1024);
								send(socketfd, buffer, read_len, 0);
								if (read_len == 0)
								{
									break;
								}
							}
							//Image was sent successfully
							printf("Image has been sent successfully\n");
							break;
						}
					case 3:
						{
						 	/* memset copies the character 0 (an unsigned char) to the first 1024 characters of the string pointed to, by the argument buffer*/
							memset(buffer, 0x00, 1024);
							printf("Enter the name of file to send along with the extension, Example: audio.mp3\nNote: the file should be in the same directory as the client.c file\n");
							scanf("%s", buffer);
							file_name_len = strlen(buffer);

							//sending file name to server
							send(socketfd, buffer, file_name_len, 0);

							//opening the file in the buffer for reading
							sourse_fd = open(buffer, O_RDONLY);
							if (!sourse_fd)
							{
							 	//error opening the file
								perror("Error : ");
								return 1;
							}

							while (1)
							{
							 	//reading the file and then sending it through buffer, if read_len is 0 then there is nothing to send and the loop gets terminated
								memset(buffer, 0x00, 1024);
								read_len = read(sourse_fd, buffer, 1024);
								send(socketfd, buffer, read_len, 0);
								if (read_len == 0)
								{
									break;
								}
							}
							//Audio file was sent successfully
							printf("Audio file has been sent\n");
							break;
						}
					case 4:
						{
						 	/* memset copies the character 0 (an unsigned char) to the first 1024 characters of the string pointed to, by the argument buffer */
							memset(buffer, 0x00, 1024);
							printf("Enter the name of the file to send along with the extension, Example: video.mp4\nNote: the file should be in the same directory as the client.c file\n");
							scanf("%s", buffer);
							file_name_len = strlen(buffer);

							//sending file name to server
							send(socketfd, buffer, file_name_len, 0);

							//opening the file in the buffer for reading
							sourse_fd = open(buffer, O_RDONLY);
							if (!sourse_fd)
							{
							 	//error opening the file
								perror("Error : ");
								return 1;
							}

							while (1)
							{
							 	//reading the file and then sending it through buffer, if read_len is 0 then there is nothing to send and the loop gets terminated
								memset(buffer, 0x00, 1024);
								read_len = read(sourse_fd, buffer, 1024);
								send(socketfd, buffer, read_len, 0);
								if (read_len == 0)
								{
									break;
								}
							}
							//Video file was sent successfully
							printf("Video sent\n");
							break;
						}
					default:
						exit(0);
				}
				break;
			}
		case 2:

			while (1)
			{

				//clearing buffer as will stream data through buffer only
				fflush(stdout);
				bzero(buffer, 1024);

				//fgets reads a line from the specified stream and stores it into the string pointed to by buffer.
				fgets(buffer, 1024, stdin);

				//sending whatever is in buffer to sever
				n = write(socketfd, buffer, strlen(buffer));

				if (n < 0)
					//file descriptor returns -1 which means write failed
					error("Error Writing");

				//again clearing buffer for reading data from server
				bzero(buffer, 512);
				//reading data from server
				n = read(socketfd, buffer, 1024);
				if (n < 0)
				{
				 		//file descriptor returns -1 which means read failed
					error("Error reading");
				}
				printf("Server: %s\n", buffer);

				//Server or client can anytime close the chat with keyword "Bye"
				int i = strncmp("Bye", buffer, 3);
				if (i == 0)
					break;
			}
			break;

		default:
			//user entered other keyword, so we have to terminate the program 
			printf("Exiting! Bye!");
			exit(0);
	}

	//closing the socket

	close(socketfd);
	return 0;
}
