hoonsy:hoonsy.o
	gcc hoonsy.o -o hoonsy
http_server.o:http_server.c
	gcc -c hoonsy.c -o hoonsy.o
.PHONY:clean
clean:
	rm -rf *.o
