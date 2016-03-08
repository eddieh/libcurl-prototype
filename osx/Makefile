CFLAGS=-Iinclude -I/opt/local/include
LDFLAGS=lib/libcurl.a -L/opt/local/lib -lidn -lssl -lcrypto -lssl -lcrypto -lz

multi-single: multi-single.c
	cc $(CFLAGS) $(LDFLAGS) -o multi-single multi-single.c

multithread: multithread.c
	cc $(CFLAGS) $(LDFLAGS) -lpthread -o multithread multithread.c

threaded-ssl: threaded-ssl.c
	cc $(CFLAGS) $(LDFLAGS) -lpthread -o threaded-ssl threaded-ssl.c
