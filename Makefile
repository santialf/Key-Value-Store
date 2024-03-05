#make file example

CC = gcc #compiler

TARGET1 = app1 #target file name
TARGET2 = app2
TARGET3 = KVS-LocalServer
TARGET4 = KVS-AuthServer

all: Application1 Application2 KVS-LocalServer KVS-AuthServer

Application1:
	$(CC) -Wall -std=c99 -O3 -pthread -o $(TARGET1) KVS-lib.c KVS-lib.h Application1.c

Application2:
	$(CC) -Wall -std=c99 -O3 -pthread -o $(TARGET2) KVS-lib.c KVS-lib.h Application2.c

KVS-LocalServer:
	$(CC) -Wall -std=c99 -O3 -pthread -o $(TARGET3) KVS-LocalServer.c list.h list.c

KVS-AuthServer:
	$(CC) -Wall -std=c99 -O3 -pthread -o $(TARGET4) authList.c authList.h KVS-AuthServer.c

clean: 
	rm -f $(TARGET1) $(TARGET2) $(TARGET3) $(TARGET4)
