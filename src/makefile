# mailParser makefile 
CC= gcc
MYSQLCFLAGS= -I/usr/include/mysql -DBIG_JOINS=1 -fno-strict-aliasing -g
MYSQLLIBS= -L/usr/lib/arm-linux-gnueabihf -lmysqlclient -lpthread -lz -lm -lrt -ldl
mailParser: mainMenu.c
	$(CC) -o mailParser $(MYSQLCFLAGS) mainMenu.c $(MYSQLLIBS)

clean:
	rm -f mailParser
