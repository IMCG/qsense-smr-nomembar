.PHONY:	all

BENCHS = src/linkedlist src/linkedlist-harris_opt src/linkedlist-harris_opt_tcmalloc src/linkedlist-harris_opt_jemalloc src/linkedlist-michael src/hashtable src/hashtable-java src/hashtable-copy src/skiplist src/skiplist-fraser src/skiplist-herlihy_lf src/skiplist-seq src/skiplist-lock src/skiplist-herlihy_lb src/skiplist-pugh src/skiplist-string-pugh src/bst src/bst-seq_internal src/bst-howley src/bst-aravind src/noise/ src/tests/ src/bst-tk/ src/linkedlist-smr-hart src/linkedlist-smr-no-membar src/linkedlist-qsbr-hart src/linkedlist-qsbr src/linkedlist-smr src/linkedlist-smr-harris_opt src/linkedlist-qsbr-harris_opt src/linkedlist-smr-no-membar-harris_opt src/linkedlist-qsbr-smr-hybrid src/linkedlist-smr-lazy src/linkedlist-smr-membar-lazy src/linkedlist-smr-no-membar-igor_opt/ src/linkedlist-hybrid-lazy
LBENCHS = src/linkedlist-lock src/hashtable-lock src/linkedlist-coupling src/linkedlist-lazy src/linkedlist-pugh src/linkedlist-copy src/hashtable-pugh src/hashtable-coupling src/hashtable-lazy src/hashtable-java src/hashtable-copy src/skiplist-lock src/skiplist-herlihy_lb src/skiplist-pugh src/skiplist-string-pugh src/bst-lock2 src/bst-drachsler src/bst-tk/ src/linkedlist-qsbr-hart src/linkedlist-qsbr src/linkedlist-qsbr-harris_opt src/linkedlist-qsbr-smr-hybrid src/linkedlist-hybrid-lazy
LFBENCHS = src/linkedlist src/linkedlist-harris_opt src/linkedlist-harris_opt_tcmalloc src/linkedlist-harris_opt_jemalloc src/linkedlist-michael src/hashtable src/skiplist src/skiplist-fraser src/skiplist-herlihy_lf src/bst src/bst-howley src/bst-aravind src/linkedlist-smr-hart src/linkedlist-smr-no-membar src/linkedlist-smr src/linkedlist-smr-harris_opt src/linkedlist-smr-no-membar-harris_opt src/linkedlist-smr-lazy src/linkedlist-smr-membar-lazy src/linkedlist-smr-no-membar-igor_opt/
SEQBENCHS = src/linkedlist-seq src/hashtable-seq src/skiplist-seq src/bst-seq_internal src/bst-seq_external
NOISE = src/noise
TESTS = src/tests
BSTS = src/bst-lock2 src/bst-drachsler src/bst src/bst-howley src/bst-aravind src/bst-tk/

.PHONY:	clean all $(BENCHS) $(LBENCHS) $(NOISE) $(TESTS) $(SEQBENCHS)

all:	lockfree tas lbhtgl

mutex:
	$(MAKE) "LOCK=MUTEX" $(LBENCHS)

spin:
	$(MAKE) "LOCK=SPIN" $(LBENCHS)

tas:
	$(MAKE) "LOCK=TAS" $(LBENCHS)

ticket:
	$(MAKE) "LOCK=TICKET" $(LBENCHS)

hticket:
	$(MAKE) "LOCK=HTICKET" $(LBENCHS)

clh:
	$(MAKE) "LOCK=CLH" $(LBENCHS)

bst:	seqbstint seqbstext
	$(MAKE) $(BSTS)

bst_tk:
	$(MAKE) "LOCK=TAS" src/bst-tk/

bst_aravind:
	$(MAKE) "STM=LOCKFREE" src/bst-aravind

bst_howley:
	$(MAKE) "STM=LOCKFREE" src/bst-howley

bst_ellen:
	$(MAKE) "STM=LOCKFREE" src/bst

bst_drachsler:
	$(MAKE) "LOCK=TAS" src/bst-drachsler

bst_drachsler_no_ro:
	$(MAKE) "LOCK=TAS" "RO_FAIL=0" src/bst-drachsler

bst_bronson:
	$(MAKE) "LOCK=TAS" src/bst-lock2

sequential:
	$(MAKE) "STM=SEQUENTIAL" "SEQ_NO_FREE=1" $(SEQBENCHS)

seqgc:
	$(MAKE) "STM=SEQUENTIAL" $(SEQBENCHS)

seq:	sequential


seqht:
	$(MAKE) "STM=SEQUENTIAL" "SEQ_NO_FREE=1" src/hashtable-seq

seqhtgc:
	$(MAKE) "STM=SEQUENTIAL" "GC=1" src/hashtable-seq

seqsl:
	$(MAKE) "STM=SEQUENTIAL" "SEQ_NO_FREE=1" src/skiplist-seq

seqslgc:
	$(MAKE) "STM=SEQUENTIAL" "GC=1" src/skiplist-seq

lockfree:
	$(MAKE) "STM=LOCKFREE" $(LFBENCHS)

noise:
	$(MAKE) $(NOISE)

tests:
	$(MAKE) $(TESTS)

lfsl:
	$(MAKE) "STM=LOCKFREE" src/skiplist

lfsl_fraser:
	$(MAKE) "STM=LOCKFREE" src/skiplist-fraser

lfsl_herlihy_lf:
	$(MAKE) "STM=LOCKFREE" src/skiplist-herlihy_lf

lbsl_pugh:
	$(MAKE) "LOCK=TAS" src/skiplist-pugh

lbsl_string_pugh:
	$(MAKE) "LOCK=TAS" src/skiplist-string-pugh

lbsl_herlihy_lb:
	$(MAKE) "LOCK=TAS" src/skiplist-herlihy_lb

sl:	seqsl lfsl_fraser lfsl_herlihy_lf lbsl_pugh lbsl_string_pugh lbsl_herlihy_lb

# OUR TARGETS
smr:
	$(MAKE) "STM=LOCKFREE" "GC=0" "INIT=one" src/linkedlist-smr-harris_opt	
qsbr:
	$(MAKE) "LOCK=TICKET" "GC=0" "INIT=one" src/linkedlist-qsbr-harris_opt
smr_no_membar:
	$(MAKE) "STM=LOCKFREE" "GC=0" "INIT=one" src/linkedlist-smr-no-membar-harris_opt

# Optimizations are enabled using IGOR_OPT_LEVEL which
# is coded on a 3-bit integer: lowest bit enables bloom, 
# second bit enables periodic_op scan, highest bit enables
# only looking at old enough nodes	
smr_no_membar_igor0:
	$(MAKE) "STM=LOCKFREE" "GC=0" "IGOR_OPT_LEVEL=0" "INIT=one" src/linkedlist-smr-no-membar-igor_opt/
smr_no_membar_igor1:
	$(MAKE) "STM=LOCKFREE" "GC=0" "IGOR_OPT_LEVEL=1" "INIT=one" src/linkedlist-smr-no-membar-igor_opt/
smr_no_membar_igor2:
	$(MAKE) "STM=LOCKFREE" "GC=0" "IGOR_OPT_LEVEL=2" "INIT=one" src/linkedlist-smr-no-membar-igor_opt/
smr_no_membar_igor3:
	$(MAKE) "STM=LOCKFREE" "GC=0" "IGOR_OPT_LEVEL=3" "INIT=one" src/linkedlist-smr-no-membar-igor_opt/
smr_no_membar_igor4:
	$(MAKE) "STM=LOCKFREE" "GC=0" "IGOR_OPT_LEVEL=4" "INIT=one" src/linkedlist-smr-no-membar-igor_opt/
smr_no_membar_igor5:
	$(MAKE) "STM=LOCKFREE" "GC=0" "IGOR_OPT_LEVEL=5" "INIT=one" src/linkedlist-smr-no-membar-igor_opt/
smr_no_membar_igor6:
	$(MAKE) "STM=LOCKFREE" "GC=0" "IGOR_OPT_LEVEL=6" "INIT=one" src/linkedlist-smr-no-membar-igor_opt/
smr_no_membar_igor7:
	$(MAKE) "STM=LOCKFREE" "GC=0" "IGOR_OPT_LEVEL=7" "INIT=one" src/linkedlist-smr-no-membar-igor_opt/

opt: smr_no_membar_igor0 smr_no_membar_igor1 smr_no_membar_igor2 smr_no_membar_igor3 smr_no_membar_igor4 smr_no_membar_igor5 smr_no_membar_igor6 smr_no_membar_igor7
hybrid:
	$(MAKE) "GC=0" "LOCK=TICKET" "INIT=one" src/linkedlist-qsbr-smr-hybrid
hybrid_lazy:
	$(MAKE) "GC=0" "LOCK=TICKET" "INIT=one" src/linkedlist-hybrid-lazy
smr_lazy:
	$(MAKE) "GC=0" "STM=LOCKFREE" "INIT=one" src/linkedlist-smr-lazy

smr_membar_lazy:
	$(MAKE) "GC=0" "STM=LOCKFREE" "INIT=one" src/linkedlist-smr-membar-lazy		
lfll_mr: smr qsbr smr_no_membar hybrid hybrid_lazy lfll_harris_opt

	
time-qsbr:
	$(MAKE) "LOCK=TICKET" "GC=0" "INIT=one" "TEST=time" src/linkedlist-qsbr-harris_opt
time-smr_no_membar:
	$(MAKE) "STM=LOCKFREE" "GC=0" "INIT=one" "TEST=time" src/linkedlist-smr-no-membar-harris_opt
time-hybrid:
	$(MAKE) "GC=0" "LOCK=TICKET" "INIT=one" "TEST=time" src/linkedlist-qsbr-smr-hybrid
time-hybrid_lazy:
	$(MAKE) "GC=0" "LOCK=TICKET" "INIT=one" "TEST=time" src/linkedlist-hybrid-lazy
time-lfll_mr: time-qsbr time-smr_no_membar time-hybrid time-hybrid_lazy



lfll_harris:
	$(MAKE) "STM=LOCKFREE" src/linkedlist

lfll_qsbr_harris:
	$(MAKE) "GC=0" "LOCK=TICKET" src/linkedlist-qsbr

lfll_smr_harris:
	$(MAKE) "GC=0" "STM=LOCKFREE" src/linkedlist-smr

lfll_harris_opt:
	$(MAKE) "GC=0" "STM=LOCKFREE" "INIT=one" src/linkedlist-harris_opt

malloc_compare: lfll_harris_opt lfll_harris_opt_tcmalloc lfll_harris_opt_jemalloc

lfll_harris_opt_malloc:
	$(MAKE) "STM=LOCKFREE" "GC=0" "MALLOC=1" src/linkedlist-harris_opt

lfll_harris_opt_tcmalloc:
	$(MAKE) "STM=LOCKFREE" src/linkedlist-harris_opt_tcmalloc

lfll_harris_opt_jemalloc:
	$(MAKE) "STM=LOCKFREE" src/linkedlist-harris_opt_jemalloc

lfll_michael:
	$(MAKE) "STM=LOCKFREE" "GC=0" src/linkedlist-michael

lfll_smr_hart:
	$(MAKE) "STM=LOCKFREE" "GC=0" src/linkedlist-smr-hart

lfll_smr_no_membar:
	$(MAKE) "STM=LOCKFREE" "GC=0" src/linkedlist-smr-no-membar	


lfll_hart_qsbr:
	$(MAKE) "GC=0" "LOCK=TICKET" src/linkedlist-qsbr-hart


seqll:
	$(MAKE) "STM=SEQUENTIAL" "SEQ_NO_FREE=1" src/linkedlist-seq

seqllgc:
	$(MAKE) "STM=SEQUENTIAL" "GC=1" src/linkedlist-seq


lfll: lfll_harris lfll_michael lfll_harris_opt

# ll: seqll lfll lbll llcopy lbll_coupling lbll_pugh lbll_lazy

ll: seqll lfll lbll llcopy lbll_coupling lbll_pugh lbll_lazy lbll_lazy_no_ro llcopy_no_ro lbll_pugh_no_ro

lbhtgl:
	$(MAKE) "LOCK=TAS" "G=GL" src/hashtable-lock

lbht:
	$(MAKE) "LOCK=TAS" src/hashtable-lock

lbht_coupling:
	$(MAKE) "LOCK=TAS" src/hashtable-coupling

lbht_pugh:
	$(MAKE) "LOCK=TAS" src/hashtable-pugh

lbht_lazy:
	$(MAKE) "LOCK=TAS" src/hashtable-lazy

lbht_coupling_gl:
	$(MAKE) "LOCK=TAS" "G=GL" src/hashtable-coupling

lbht_pugh_gl:
	$(MAKE) "LOCK=TAS" "G=GL" src/hashtable-pugh

lbht_pugh_gl_no_ro:
	$(MAKE) "LOCK=TAS" "G=GL" "RO_FAIL=0" src/hashtable-pugh

lbht_lazy_gl:
	$(MAKE) "LOCK=TAS" "G=GL" src/hashtable-lazy

lbht_lazy_gl_no_ro:
	$(MAKE) "LOCK=TAS" "G=GL" "RO_FAIL=0" src/hashtable-lazy

lbll_coupling:
	$(MAKE) "LOCK=TAS" src/linkedlist-coupling

lbll_pugh:
	$(MAKE) "LOCK=TAS" src/linkedlist-pugh

lbll_pugh_no_ro:
	$(MAKE) "LOCK=TAS" "RO_FAIL=0" src/linkedlist-pugh

lbll_lazy:
	$(MAKE) "LOCK=TAS" src/linkedlist-lazy

lbll_lazy_no_ro:
	$(MAKE) "LOCK=TAS" "RO_FAIL=0" src/linkedlist-lazy

lbll:
	$(MAKE) "LOCK=TAS" src/linkedlist-lock

lbllclh:
	$(MAKE) "LOCK=TAS" src/linkedlist-lock

llcopy:
	$(MAKE) "LOCK=CLH" src/linkedlist-copy

llcopy_no_ro:
	$(MAKE) "LOCK=CLH" "RO_FAIL=0" src/linkedlist-copy

htcopy:
	$(MAKE) "LOCK=TAS" src/hashtable-copy

htcopy_no_ro:
	$(MAKE) "LOCK=TAS" "RO_FAIL=0" src/hashtable-copy

htcopygl:
	$(MAKE) "LOCK=CLH" "G=GL" src/hashtable-copy

lfht:
	$(MAKE) "STM=LOCKFREE" src/hashtable

lbsl:
	$(MAKE) "LOCK=TAS" src/skiplist-lock

htjava:
	$(MAKE) "LOCK=TAS" src/hashtable-java

htjava_no_ro:
	$(MAKE) "LOCK=TAS" "RO_FAIL=0" src/hashtable-java

ht:	seqht lfht lbht lbhtgl htjava htcopy lbht_coupling lbht_lazy lbht_pugh lbht_coupling_gl lbht_lazy_gl lbht_pugh_gl lbht_lazy_gl_no_ro lbht_pugh_gl_no_ro htcopy_no_ro htjava_no_ro

seqbstint:
	$(MAKE) "STM=SEQUENTIAL" "SEQ_NO_FREE=1" src/bst-seq_internal

seqbstext:
	$(MAKE) "STM=SEQUENTIAL" "SEQ_NO_FREE=1" src/bst-seq_external

seqbstintgc:
	$(MAKE) "STM=SEQUENTIAL" "GC=1" src/bst-seq_internal

seqbstextgc:
	$(MAKE) "STM=SEQUENTIAL" "GC=1" src/bst-seq_external

seqbstgc: seqbstextgc seqbstintgc

clean:
	$(MAKE) -C src/linkedlist clean	
	$(MAKE) -C src/skiplist clean
	$(MAKE) -C src/hashtable clean
	# $(MAKE) -C src/hashtable-rcu clean
	$(MAKE) -C src/rbtree clean
	$(MAKE) -C src/linkedlist-coupling clean
	$(MAKE) -C src/linkedlist-lock clean
	$(MAKE) -C src/linkedlist-lazy clean
	$(MAKE) -C src/linkedlist-pugh clean
	$(MAKE) -C src/hashtable-pugh clean
	$(MAKE) -C src/hashtable-lock clean
	$(MAKE) -C src/hashtable-coupling clean
	$(MAKE) -C src/hashtable-lazy clean
	$(MAKE) -C src/skiplist-lock clean
	# $(MAKE) -C src/sftree clean
	$(MAKE) -C src/bst clean
	$(MAKE) -C src/bst-howley clean
	$(MAKE) -C src/bst-aravind clean
	$(MAKE) -C src/bst-drachsler clean
	$(MAKE) -C src/bst-lock2 clean
	$(MAKE) -C src/bst-tk clean
	$(MAKE) -C src/deque clean
	$(MAKE) -C src/noise clean
	$(MAKE) -C src/tests clean
	rm -rf build

$(SEQBENCHS):
	$(MAKE) -C $@ $(TARGET)

$(LBENCHS):
	$(MAKE) -C $@ $(TARGET)

$(LFBENCHS):
	$(MAKE) -C $@ $(TARGET)

$(NOISE):
	$(MAKE) -C $@ $(TARGET)

$(TESTS):
	$(MAKE) -C $@ $(TARGET)
