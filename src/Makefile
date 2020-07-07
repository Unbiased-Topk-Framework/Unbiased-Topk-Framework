CC = g++
CFLAGS = -O2 -std=gnu++14

OBJS = UTF_freq_topk.out UTF_hg_topk.out UTF_ra_topk.out UTF_ss_topk.out USS.out

all: $(OBJS)

UTF_hg_topk.out: UTF_hg_topk.cpp
	$(CC) $(CFLAGS) -o UTF_hg_topk.out UTF_hg_topk.cpp

UTF_ss_topk.out: UTF_ss_topk.cpp
	$(CC) $(CFLAGS) -o UTF_ss_topk.out UTF_ss_topk.cpp

UTF_ra_topk.out: UTF_ra_topk.cpp
	$(CC) $(CFLAGS) -o UTF_ra_topk.out UTF_ra_topk.cpp

UTF_freq_topk.out: UTF_freq_topk.cpp
	$(CC) $(CFLAGS) -o UTF_freq_topk.out UTF_freq_topk.cpp

USS.out: USS.cpp
	$(CC) $(CFLAGS) -o USS.out USS.cpp

clean:
	rm -f $(OBJS)