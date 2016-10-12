all: Server.o Client.o Constants.o
	gcc Server.o Constants.o -o Executables/Server
	gcc Client.o Constants.o -o Executables/Client

Server: Server.o Constants.o
	gcc Server.o Constants.o -o Executables/Server
	
Client: Client.o Constants.o
	gcc Client.o Constants.o -o Executables/Client

Server.o:
	gcc -g -c Server/Server.c Constants.c Constants.h

Client.o:
	gcc -g -c Client/Client.c Constants.c Constants.h

Constants.o:
	gcc -g -c Constants.c Constants.h

clean:
	rm *.o Executables/*