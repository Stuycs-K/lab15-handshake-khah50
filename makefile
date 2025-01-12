all: compile

compile: client_build server_build

client_build: basic_client.o pipe_networking.o
	gcc -o client basic_client.o pipe_networking.o

server_build: basic_server.o pipe_networking.o
	gcc -o server basic_server.o pipe_networking.o

forking_server_build: forking_server.o pipe_networking.o
	gcc -o forking_server forking_server.o pipe_networking.o

client: client_build
	./client

server: forking_server_build
	./forking_server

basic_client.o: basic_client.c pipe_networking.h
	gcc -c basic_client.c

basic_server.o: basic_server.c pipe_networking.h
	gcc -c basic_server.c

forking_server.o: forking_server.c pipe_networking.h
	gcc -c forking_server.c

pipe_networking.o: pipe_networking.c pipe_networking.h
	gcc -c pipe_networking.c

clean:
	rm *.o
	rm *~
