all: Server.o Client.o Constants.o ErrorHandling.o
	gcc Server.o Constants.o ErrorHandling.o -o bsb_svr
	gcc Client.o Constants.o ErrorHandling.o -o bsb_cli

Server: Server.o Constants.o ErrorHandling.o
	gcc Server.o Constants.o ErrorHandling.o -o Executables/Server
	
Client: Client.o Constants.o ErrorHandling.o
	gcc Client.o Constants.o ErrorHandling.o -o Executables/Client

Server.o:
	gcc -g -c Server/Server.c Constants.c Constants.h ErrorHandling.c ErrorHandling.h

Client.o:
	gcc -g -c Client/Client.c Constants.c Constants.h ErrorHandling.c ErrorHandling.h

Constants.o:
	gcc -g -c Constants.c Constants.h
	
ErrorHandling.o:
	gcc -g -c ErrorHandling.c ErrorHandling.h

clean:
	rm *.o bsb_svr bsb_cli