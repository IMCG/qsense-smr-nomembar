ROOT = ../..

include $(ROOT)/common/Makefile.common

BINS = $(BINDIR)/lf-ll_harris_opt_tcmalloc
PROF = $(ROOT)/src

.PHONY:	all clean

all:	main

LIST_IMPL = harris_opt

CFLAGS += -fno-omit-frame-pointer -fno-builtin-malloc -fno-builtin-calloc -fno-builtin-realloc -fno-builtin-free -I${TCMALLOC_PATH}/include

LDFLAGS += -L${TCMALLOC_PATH}/lib -Wl,-rpath,${TCMALLOC_PATH}/lib -ltcmalloc 


measurements.o:
	$(CC) $(CFLAGS) -c -o $(BUILDIR)/measurements.o $(PROF)/measurements.c

# ssalloc.o:
# 	$(CC) $(CFLAGS) -c -o $(BUILDIR)/ssalloc.o $(PROF)/ssalloc.c

linkedlist.o: 
	$(CC) $(CFLAGS) -c -o $(BUILDIR)/linkedlist.o linkedlist.c $(LDFLAGS)

harris.o: linkedlist.h linkedlist.o
	$(CC) $(CFLAGS) -c -o $(BUILDIR)/harris.o $(LIST_IMPL).c

intset.o: linkedlist.h harris.h
	$(CC) $(CFLAGS) -c -o $(BUILDIR)/intset.o intset.c

test.o: linkedlist.h harris.h intset.h
	$(CC) $(CFLAGS) -c -o $(BUILDIR)/test.o $(TEST_FILE)

main: measurements.o linkedlist.o harris.o harris.o intset.o test.o $(TMILB)
	$(CC) $(CFLAGS) $(BUILDIR)/measurements.o $(BUILDIR)/linkedlist.o $(BUILDIR)/harris.o $(BUILDIR)/intset.o $(BUILDIR)/test.o -o $(BINS) $(LDFLAGS)

clean:
	-rm -f $(BINS)
