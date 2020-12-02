all: bin bin/server bin/client
	
bin:
	mkdir bin

bin/server: server.c
	gcc server.c -o bin/server -Wall -Werror -lm -fsanitize=undefined -fsanitize=address

bin/client: client.c
	gcc client.c -o bin/client -Wall -Werror -lm -fsanitize=undefined -fsanitize=address

clean:
	rm bin/server bin/client
	rmdir bin
