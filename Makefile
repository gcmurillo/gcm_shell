CC=gcc

all:
	$(CC) src/main.c -o gcm_shell -lpthread

clean:
	rm gcm_shell