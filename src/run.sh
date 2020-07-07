#!/bin/bash/

make

./UTF_ra_topk.out test_cases.txt
./UTF_hg_topk.out test_cases.txt
./UTF_fq_topk.out test_cases.txt
./UTF_ss_topk.out test_cases.txt
./USS.out test_cases.txt