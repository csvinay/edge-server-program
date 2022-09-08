/*
As per the given assignment, the following are the requirements:

[I] Server Model should have following functions
1) Socket
2) Bind
3) listen
4) Accept
5) Read
6) Write
7) close

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
#include <errno.h>

//Error function to exit the program
void error(const char *msg)
{
	perror(msg); /*perror is an inbuilt function that interprets the error no. and outputs the output error description by using STDERR */
	exit(1);
}

int main(int argc, char *argv[])
{
	/*
	The user need to provide 2 arguments, 
	(1)the file name - argv[0]
	(3)the port no. - argv[1]
	*/

	//checking whether all the inputs are given or not
	if (argc < 2)
	{
		//Terminating the program if condition not satisfied
		fprintf(stderr, "Please provide a port number, program is terminated\n");
		exit(1);
	}

	/*
	socketfd = file descriptor
	newsocketfd = for client
	*/
	int socketfd, newsocketfd, portno, n;

	//file name variable
	char str[20];
	int client_len, read_len, file_read_len;
	int des_fd;

	//variables for operation - send file, chat etc.
	int received_int, sub_received_int;
	int operation, return_status, sub_return_status;
	int sub_operation;

	//buffer will be transferred in a data stream
	char buffer[1024];

	//sockaddr_in gives internet address
	struct sockaddr_in serv_address, client_address;
	socklen_t clilen;

	/*socket function has 3 arguments, 
	(1) domain, here we will use AF_INET which is an IPv4 protocol,
	(2) type, we will use SOCK_STREAM as it for TCP protocol,
	(3) protocol, which is 0 for TCP 
	*/
	socketfd = socket(AF_INET, SOCK_STREAM, 0);

	if (socketfd < 0)
	{
		error("There is an error opening the socket");
	}

	//bzero clears data or anything in whatever it is refrenced to
	//clearing serv_address so as to do operations on it 
	bzero((char*) &serv_address, sizeof(serv_address));

	//converting the string argument into an int
	portno = atoi(argv[1]);

	/*getting info such as port, using htons - host to network short,
	converts port no. in host byte order to port no. in network byte order */
	serv_address.sin_family = AF_INET;
	serv_address.sin_addr.s_addr = INADDR_ANY;
	serv_address.sin_port = htons(portno);

	/*bind assigns the address specified by addr */
	if (bind(socketfd, (struct sockaddr *) &serv_address, sizeof(serv_address)) < 0)
		error("Binding Failed");

	/*listen makes the socketfd as passive soocket i.e. A socket that will be used for accepting incoming connections using accept */
	listen(socketfd, 5);
	clilen = sizeof(client_address);

	/*accept function also returns a file descriptor and waits for connect and whenever connect is triggered by client, accept is called by host */
	newsocketfd = accept(socketfd, (struct sockaddr *) &client_address, &clilen);

	if (newsocketfd < 0)
		error("There is an error in accepting");

	/*getting the operation by the user */
	return_status = read(newsocketfd, &received_int, sizeof(received_int));
	if (return_status > 0)
	{
		//printing the opeartion code
		fprintf(stdout, "Code of the Operation to be performed is = %d\n", ntohl(received_int));
		operation = ntohl(received_int);
	}

	//performing the operation according to user
	switch (operation)
	{
		case 1:
			//user selected send file
			{
			 	//asking user which type of file he/she wants to send
				sub_return_status = read(newsocketfd, &sub_received_int, sizeof(sub_received_int));
				if (return_status > 0)
				{
				 		//printing the sub operation code
					fprintf(stdout, "Sub Operation code = %d\n", ntohl(sub_received_int));
					sub_operation = ntohl(sub_received_int);
				}

				switch (sub_operation)
				{
				 		//performing the sub operation according to user
					case 1:

						{

						 	//User selected txt file
							//reading the filename given by user
							bzero(str, 20);
							n = read(newsocketfd, str, 20);
							int len = strlen(str);

							//manupilating the file name, the last 4 chars are the extension, so removing the extension from the name
							str[len - 4] = '\0';
							if (n < 0)
								error("Error Reading");

							fprintf(stdout, "Received File Name = %s\n", str);

							//Starting the operation to receive the txt file
							FILE * fp;
							int ch = 0;
							fp = fopen(strcat(str, "_received.txt"), "a");
							int words;
							read(newsocketfd, &words, sizeof(int));
							

							/*
							Receiving the file until there are no words left
							*/
							//So we will keep sending the file through buffer until EOF is encountered
							while (ch != words)
							{
								read(newsocketfd, buffer, 512);
								fprintf(fp, " %s", buffer);
								ch++;
							}
							printf("received file successfully\n");
							printf("The file is saved as %s\n", str);

							break;
						}
					case 2:
						{

							char file_name[1024];	// local val

							/*memset copies the character 0 (an unsigned char) to the first 1024 characters of the string pointed to, by the argument buffer */
							memset(buffer, 0x00, 1024);

							/*file name */
							read_len = read(newsocketfd, buffer, 1024);
							if (read_len > 0)
							{
							/*getting the file name and manupilating the file name, the last 4 chars are the extension, so removing the extension from the name*/
								strcpy(file_name, buffer);
								int len = strlen(file_name);
								file_name[len - 4] = '\0';
							}
							else
							{
							 	//Error reading the file name, terminating the program
								close(newsocketfd);
								break;
							}

							/*create file */

							des_fd = open(strcat(file_name, "received.png"), O_WRONLY | O_CREAT | O_EXCL, 0700);
							if (!des_fd)
							{
								perror("file open error : ");
								break;
							}
							/*file save */
							while (1)
							{
								memset(buffer, 0x00, 1024);
								file_read_len = read(newsocketfd, buffer, 1024);
								write(des_fd, buffer, file_read_len);
								if (file_read_len == EOF | file_read_len == 0)
								{
								 	//if block executed when EOF is encountered, meaning there is nothing left to receive
									//The operation executed successfully
									//The file was recived successfully
									printf("Received Image file successfully\n");
									printf("The file is saved as %s\n", file_name);
									break;
								}
							}
							break;
						}
					case 3:
						{
							char file_name[1024];	// local val

							/*memset copies the character 0 (an unsigned char) to the first 1024 characters of the string pointed to, by the argument buffer */
							memset(buffer, 0x00, 1024);

							/*file name */
							read_len = read(newsocketfd, buffer, 1024);
							if (read_len > 0)
							{
							 	/* getting the file name and manupilating the file name, the last 4 chars are the extension, so removing the extension from the name */
								strcpy(file_name, buffer);
								int len = strlen(file_name);
								file_name[len - 4] = '\0';
							}
							else
							{
							 	//Error reading the file name, terminating the program
								close(newsocketfd);
								break;
							}

							/*create file */

							des_fd = open(strcat(file_name, "received.mp3"), O_WRONLY | O_CREAT | O_EXCL, 0700);
							if (!des_fd)
							{

								perror("file open error : ");
								break;
							}
							/*file save */
							while (1)
							{
								memset(buffer, 0x00, 1024);
								file_read_len = read(newsocketfd, buffer, 1024);
								write(des_fd, buffer, file_read_len);
								if (file_read_len == EOF | file_read_len == 0)
								{
								 	//if block executed when EOF is encountered, meaning there is nothing left to receive
									//The operation executed successfully
									//The file was recived successfully
									printf("Received audio file successfully\n");
									printf("The file is saved as %s\n", file_name);
									break;
								}
							}
							break;
						}

					case 4:
						{
							char file_name[1024];	// local val

							/*memset copies the character 0 (an unsigned char) to the first 1024 characters of the string pointed to, by the argument buffer */
							memset(buffer, 0x00, 1024);

							/*file name */
							read_len = read(newsocketfd, buffer, 1024);
							if (read_len > 0)
							{
							 	/* getting the file name and manupilating the file name, the last 4 chars are the extension, so removing the extension from the name */
								strcpy(file_name, buffer);
								int len = strlen(file_name);
								file_name[len - 4] = '\0';
							}
							else
							{
							 	//Error reading the file name, terminating the program
								close(newsocketfd);
								break;
							}

							/*create file */

							des_fd = open(strcat(file_name, "received.mp4"), O_WRONLY | O_CREAT | O_EXCL, 0700);
							if (!des_fd)
							{

								perror("file open error : ");
								break;
							}
							/*file save */
							while (1)
							{
								memset(buffer, 0x00, 1024);
								file_read_len = read(newsocketfd, buffer, 1024);
								write(des_fd, buffer, file_read_len);
								if (file_read_len == EOF | file_read_len == 0)
								{
								 	//if block executed when EOF is encountered, meaning there is nothing left to receive
									//The operation executed successfully
									//The file was recived successfully
									printf("Received Video file successfully\n");
									printf("The file is saved as %s\n", file_name);
									break;
								}
							}
							break;
						}
					default:
						exit(0);
				}
				break;
			}
		case 2:
			//user selected to chat
			while (1)
			{
			 	//clearing buffer as will stream data through buffer only
				bzero(buffer, 1024);

				//reading data from client
				n = read(newsocketfd, buffer, 1024);
				if (n < 0)
					error("Error Reading");
				printf("Client: %s\n", buffer);
				bzero(buffer, 1024);
				//fgets reads a line from the specified stream and stores it into the string pointed to by buffer.
				fgets(buffer, 1024, stdin);

				////sending whatever is in buffer to client
				n = write(newsocketfd, buffer, strlen(buffer));
				if (n < 0)
					error("Error Writing");

				//Server or client can anytime close the chat with keyword "Bye"
				int i = strncmp("Bye", buffer, 3);
				if (i == 0)
					break;
			}
			break;
		default:
			//user entered other keyword, so we have to terminate the program 
			printf("Exiting! Bye!\n");
			exit(0);
	}

	//closing the socket
	close(newsocketfd);
	close(socketfd);
	return 0;
}
