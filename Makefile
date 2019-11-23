CC=gcc

all:
	$(CC) src/main.c -o gcm_shell

clean:
	rm gcm_shell