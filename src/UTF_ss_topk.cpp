#include <iostream>
#include <algorithm>
#include <fstream> 
#include <stdio.h>
#include <string.h>
#include <cmath> 
#include <string>
#include <cstdlib>
#include <unordered_map>
#include "BOBHash32.h"
#include "data.h"
#include "util.h"
#include "cmm_sketch.h"
#include "framework.h"
#include <cassert>
using namespace std;

class Sketch_ss: public Framework{ // 20 bytes per item
public:
	void insert(int in);
	Sketch_ss(double a, double c, int hNum, int bNum, int blen, int cbNum, int crNum);
};

Sketch_ss::Sketch_ss(double a = 0.07, double c = 1 / 1.08, int hNum = 1, int bNum = 256, int blen = 8, int cbNum = 20480, int crNum = 1){
	light_part = new CMMSketch(cbNum, crNum);
	light_part2 = new CMMSketch(cbNum, crNum);

	cons = c;
	alpha = a;
	bucketNum = bNum;
	bucket_length = blen;
	hashNum = hNum;
	for(int i = 0; i < bucketNum; i++){
		for (int j = 0; j < bucket_length; j++){
            item[i][j].id = 0;
		    item[i][j].cnt = 0;
			item[i][j].underest = 0;
			item[i][j].store_unbiased = 0;
			item[i][j].store_overest = 0;
        }
	}
	hash_id = new BOBHash32(rand() % MAX_PRIME32);
	hash_cnt = new BOBHash32[hashNum];
	for(int i = 0; i < hashNum; i++)
		hash_cnt[i].initialize(rand() % MAX_PRIME32);
}

void Sketch_ss::insert(int in){
	unsigned int id_num = in;
	char c2[4] = {};
	memcpy((void*)c2, &in, 4);
	#ifdef DEBUG_MODE
		light_part2->insert(c2, 1);
	#endif
	unsigned int h_id = hash_cnt[0].run(c2, 4) % bucketNum; 

	int i, j;
    int min_cnt = INF, arg_min = -1;

	for (j = 0; j < bucket_length; j++){
        if ((item[h_id][j].id == id_num) && (item[h_id][j].underest >= 1)){
            item[h_id][j].cnt++;
			item[h_id][j].underest++;
            return;
        }
    }

    for (j = 0; j < bucket_length; j++){
        if (min_cnt > item[h_id][j].cnt){
            min_cnt = item[h_id][j].cnt;
            arg_min = j;
        }
    }

    for (j = 0; j < bucket_length; j++){
        if (item[h_id][j].cnt == 0){
            item[h_id][j].id = id_num;
            item[h_id][j].cnt = 1;
			item[h_id][j].underest = 1;
			item[h_id][j].store_unbiased = light_part->query(c2, 1);
			item[h_id][j].store_overest = light_part->query(c2, 0);
            return;
        }
    }

    char c_old[4] = {};
    memcpy((void*)c_old, &item[h_id][arg_min].id, 4);

	item[h_id][arg_min].cnt++;
    light_part->insert(c_old, item[h_id][arg_min].underest);
	item[h_id][arg_min].underest = 1;
	item[h_id][arg_min].id = id_num;
	item[h_id][arg_min].store_unbiased = light_part->query(c2, 1);
	item[h_id][arg_min].store_overest = light_part->query(c2, 0);
}

Sketch_ss* s; 

FILE* fout;

void test_topk(int bucket_length, int topk, int rep_time, int tot_flow, int heavy_bucnum, int light_bucnum){	
	int i, j;
	double ARE_under, ARE_unbiased, ARE_over, ARE, AAE_unbiased, throughput;
	int under_es, over_es, tot_deviate, equal;

	double avg_AAE_unbiased = 0, avg_ARE_under = 0, avg_ARE_unbiased = 0, avg_ARE_over = 0, avg_f1 = 0, avg_under = 0, avg_over = 0, avg_equal = 0, avg_deviate = 0, avg_badcase = 0, avg_throughput = 0;

	for (int t = 0; t < rep_time; t++){
		printf("%d ", t);

		s = new Sketch_ss(1, 1. / 1.08, 1, heavy_bucnum, bucket_length, light_bucnum);

		TP start, finish;
		start = now();
		for (i = 0; i < tot_flow; i++){
			s->insert(hashed_value[i]);
		}
		finish = now();
		throughput = (double) tot_flow / std::chrono::duration_cast<std::chrono::duration<double,std::ratio<1,1000000> > >(finish - start).count();
		avg_throughput += throughput / rep_time;

		// CMM sketch
		#ifdef DEBUG_MODE
			ARE = 0;
			under_es = 0, over_es = 0, tot_deviate = 0, equal = 0;
			for (i = 0; i < topk; i++){
				int res = s->query_cs(flow[i].hash_value);
				tot_deviate += res - flow[i].cnt;
				ARE += fabs(res - flow[i].cnt) / (double)(flow[i].cnt);
				if (res > flow[i].cnt){
					over_es++;
				}
				else if (res < flow[i].cnt){
					under_es++;
				}
				else{
					equal++;
				}
			}
			printf("%f %d %d %d %d\n", ARE / (double)topk, under_es, over_es, equal, tot_deviate);
		#endif

		// UTF
		ARE_under = ARE_over = ARE_unbiased = AAE_unbiased = 0;
		under_es = over_es = tot_deviate = 0;

		s->query_init(topk);

		int hit = 0, tmp_hit;
		int badcase = 0;
		equal = 0;
		for (i = 0; i < topk; i++){
			int res = s->query_topk(flow[i].hash_value, tmp_hit, 1, 0);

			hit += tmp_hit;
			if (tmp_hit){
				tot_deviate += res - flow[i].cnt;
				AAE_unbiased += fabs(res - flow[i].cnt);
				ARE_unbiased += fabs(res - flow[i].cnt) / (double)(flow[i].cnt);
				if (fabs(res - flow[i].cnt) / (double)(flow[i].cnt) > 0.2){
					badcase++;
				}

				if (res > flow[i].cnt){
				over_es++;
				}
				else if (res < flow[i].cnt){
					under_es++;
				}
				else{
					equal++;
				}
			}

			res = s->query_topk(flow[i].hash_value, tmp_hit, 0, 0);
			if (tmp_hit){
				ARE_under += fabs(res - flow[i].cnt) / (double)(flow[i].cnt);
			}
			res = s->query_topk(flow[i].hash_value, tmp_hit, 2, 0);
			if (tmp_hit){
				ARE_over += fabs(res - flow[i].cnt) / (double)(flow[i].cnt);
			}
		}
		avg_AAE_unbiased += AAE_unbiased / (double)hit / rep_time;
		avg_ARE_unbiased += ARE_unbiased / (double)hit / rep_time;
		avg_ARE_under += ARE_under / (double)hit / rep_time;
		avg_ARE_over += ARE_over / (double)hit / rep_time;
		avg_under += (double)under_es / rep_time;
		avg_over += (double)over_es / rep_time;
		avg_equal += (double)equal / rep_time;
		avg_deviate += (double)tot_deviate / rep_time;
		avg_f1 += (double)hit / (double)topk / rep_time;
		avg_badcase += (double)badcase / rep_time;

		delete(s);
	}
	fprintf(fout, "%.9f %.9f %.9f %.9f %.9f %.9f %.9f %.9f %.9f %.9f %.9f %.9f\n", avg_throughput, avg_AAE_unbiased, avg_ARE_under, avg_ARE_unbiased, avg_ARE_over, avg_under, avg_over, avg_equal, avg_deviate, avg_f1, avg_badcase);
	printf("\n");
}

int main(int argc, const char* argv[]){
	int memory, percent, topk_val, blen;
	int n_case = 0, dataset_id;
	int tot_flow_cnt;
	char exp_name[100];
	FILE* fin = fopen(argv[1], "r");

	fscanf(fin, "%d %d %s", &n_case, &dataset_id, exp_name);
	fout = fopen(exp_name, "a+");
	printf("UTF + SpaceSaving:\n");
	fprintf(fout, "UTF + SpaceSaving:\n");

	srand(time(NULL));
	printf("start loading\n");
	tot_flow_cnt = load_data(TOTAL_FLOW, dataset_id);
	printf("end loading\n");

	for (int t = 0; t < n_case; t++){
		printf("Test case #%d\n", t);
		fscanf(fin, "%d %d %d %d", &memory, &percent, &topk_val, &blen);
		
		int heavy_bucnum = memory * percent * 1024 /1600 / blen; // 16 bytes per item
		int light_bucnum = memory * (100 - percent) * 1024 / 200; // 2 bytes per item
		
		test_topk(blen, topk_val, REP_TIME, min(TOTAL_FLOW, tot_flow_cnt), heavy_bucnum, light_bucnum);
		printf("\n");
	}
	fclose(fin);
    fclose(fout);
	return 0;
} 
