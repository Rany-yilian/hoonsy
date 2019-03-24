http_server:http_server.o
	gcc http_server.o -o http_server
http_server.o:http_server.c
	gcc -c http_server.c -o http_server.o
.PHONY:clean
clean:
	rm -rf *.o
