all: src/server src/client src/cgi-bin/user
	
src/server: src/server.c
	gcc src/server.c -o src/server 

src/client: src/client.c
	gcc src/client.c -o src/client 

src/cgi-bin/user: src/cgi-bin/user.c
	gcc src/cgi-bin/user.c -o src/cgi-bin/user

clean:
	rm src/server src/client
