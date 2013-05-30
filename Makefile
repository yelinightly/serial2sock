CC=arm-linux-gcc
tower_monitor: *.c
	$(CC) *.c -lpthread -lm -static -o tower_monitor
clean:
	rm -f tower_monitor
