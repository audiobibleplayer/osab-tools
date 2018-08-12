makemenu: makemenu.o
	gcc -o makemenu makemenu.c
	strip makemenu

makemenu32: makemenu.o
	gcc -m32 -o makemenu makemenu.c
	strip makemenu

debug: makemenu.o
	gcc -o makemenu makemenu.c -g

clean:
	rm makemenu *.o

install: makemenu upload vs-chap toogg
	sudo cp -v $^ /usr/local/bin/

uninstall: /usr/local/bin/makemenu /usr/local/bin/upload /usr/local/bin/vs-chap /usr/local/bin/toogg
	sudo rm -f $^
