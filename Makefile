multiChatclient: multiChatclient.o
	gcc -pthread -o multiChatclient multiChatclient.o
multiChatserver: multiChatserver.o
	gcc -pthread -o multiChatserver multiChatserver.o
multiChatclient.o: multiChatclient.c
	gcc -c multiChatclient.c
multiChatserver.o: multiChatserver.c
	gcc -c multiChatserver.c
all: multiChatclient multiChatserver
clean:
	rm -f *.o multiChatserver multiChatclient
