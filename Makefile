
getdents_ls:
	gcc -g -o getdents_ls src/getdents_ls.cpp
clean:
	rm getdents_ls
install:
	mkdir -p $(DESTDIR)/usr/bin
	install -m 0755 getdents_ls $(DESTDIR)/usr/bin/getdents_ls
