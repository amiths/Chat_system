
CNT5106c (Computer Networks)
Project

Team Members : Amith Sreedhar
		Sanjay Nair

Programming language used : C

Files present : server.c
		 client.c
		Makefile

How to run : The binary files already present in the zip file can be used. To generate new binary files,
	     run the following commands:
	     run:	make all
	      First, run the server executable along with a port number of your choice.
	      Make sure port number is a higher 4 digit number to avoid using reserved ports.
	     $ ./server 4000

	      Then, run the client executable for each client with the same port number
	      as that of the server.
	     $ ./client 4000

	     You will then be asked to enter a username for the client. Make sure you give
	     a unique name.

COMMANDS :  Clients can use the following commands to perform different operations:
	     
	     Broadcast : Broadcast a message to all other clients.
	     Private   : Send a message to one particular client (Unicast).
	     Blockcast : Send a message to all other clients except one.
	     FileBroad : Broadcast a file to all other clients.
	     FileUni   : Unicast a file to a specific client.
	     FileBlock : Broadcast a file to all clients except one. 

	     Occassinally, there might be a messag saying Message from in the client terminal. This is because of a message recieved through a listener thread of the client 		     which gets printed on the screen of the client. However, the user can go ahead and type one of the above mentioned commands to continue operation.

Contributions:

Amith Sreedhar ( ufid : 33351813)
Worked on the File sending interface and operations.

Sanjay Nair:	(ufid: 85543944)
Worked on the message sending interface and operations, error handling.

Notes:
	    *Press control+C in a client’s program to remove it from the chat group.
	    *To mimic TCP operation, File transfer is implemented by transferring a 
	     small chunk of file data multiple times. Hence, there is no limit on the
	     size of file that can be transmitted over the network. Large files may take
	     time to be transmitted.
	    *Every client that enters the chat system, the server creates a directory for that user in the path <current_dir>/clients/
