lib = -lpthread

all: server.c client.c 
	gcc -o server server.c $(lib) -w
	gcc -o client client.c $(lib) -w
clean:
	rm server client 
