all: dwifi

dwifi: ./src/dwifi.o ./src/wifinet.o ./src/creator.o
			gcc ./src/dwifi.o ./src/wifinet.o ./src/creator.o -o ./dwifi

creator.o: ./src/creator.c
			gcc -c ./src/creator.c -o ./src/creator.o

wifinet.o: ./src/wifinet.c
			gcc -c ./src/wifinet.c -o ./src/wifinet.o

dwifi.o: ./src/dwifi.c
			gcc -c ./src/dwifi.c -o ./src/dwifi.o

clean: 
		rm -f ./src/dwifi.o ./src/wifinet.o ./src/creator.o
		rm -f ./dwifi

install:
		mkdir -p /etc/wifi
		cp ./dwifi /usr/bin/dwifi
		cp ./dwifid /usr/bin/dwifid
		cp ./dwifid-init-d /etc/init.d/dwifid
		touch /etc/wifi/.networks
		update-rc.d dwifid defaults

remove:
		rm -f /usr/bin/dwifi
		rm -f /usr/bin/dwifid
		update-rc.d dwifid remove

purge:
		rm -f /usr/bin/dwifi
		rm -f /etc/wifi
