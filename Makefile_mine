mine_client: mine_client.o client_struct.o
	gcc -o mine_client mine_client.o client_struct.o
mine_client.o: mine_client.c
	gcc -c mine_client.c
client_struct.o: client_struct.c
	gcc -c client_struct.c
all: mine_client
clean:
	rm -f *.o mine_client
