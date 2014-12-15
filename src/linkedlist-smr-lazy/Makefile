ROOT = ../..

include $(ROOT)/common/Makefile.common

BINS = $(BINDIR)/lf-ll_smr_lazy
PROF = $(ROOT)/src

.PHONY:	all clean

all:	main

LIST_IMPL = harris_opt

measurements.o:
	$(CC) $(CFLAGS) -c -o $(BUILDIR)/measurements.o $(PROF)/measurements.c

ssalloc.o:
	$(CC) $(CFLAGS) -c -o $(BUILDIR)/ssalloc.o $(PROF)/ssalloc.c

double_llist.o:
	$(CC) $(CFLAGS) -c -o $(BUILDIR)/double_llist.o $(PROF)/double_llist.c

smr-lazy.o:
	$(CC) $(CFLAGS) -c -o $(BUILDIR)/smr-lazy.o $(PROF)/smr_lazy.c

linkedlist.o: 
	$(CC) $(CFLAGS) -c -o $(BUILDIR)/linkedlist.o linkedlist.c

harris.o: linkedlist.h linkedlist.o smr-lazy.o
	$(CC) $(CFLAGS) -c -o $(BUILDIR)/harris.o $(LIST_IMPL).c

intset.o: linkedlist.h harris.h
	$(CC) $(CFLAGS) -c -o $(BUILDIR)/intset.o intset.c

test.o: linkedlist.h harris.h intset.h
	$(CC) $(CFLAGS) -c -o $(BUILDIR)/test.o $(TEST_FILE)

main: measurements.o ssalloc.o double_llist.o linkedlist.o harris.o harris.o intset.o test.o smr-lazy.o $(TMILB)
	$(CC) $(CFLAGS) $(BUILDIR)/measurements.o $(BUILDIR)/ssalloc.o $(BUILDIR)/double_llist.o $(BUILDIR)/linkedlist.o $(BUILDIR)/harris.o $(BUILDIR)/intset.o $(BUILDIR)/test.o $(BUILDIR)/smr-lazy.o -o $(BINS) $(LDFLAGS)

clean:
	-rm -f $(BINS)