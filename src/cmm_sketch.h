#include "BOBHash32.h"
#include "data.h"
#include "util.h"
#include <cassert>
#ifndef CMM_SKETCH_H
#define CMM_SKETCH_H
using namespace std;

class CMMSketch{
public:
	int bucketNum;
	int rowNum;
	static const int MAX_rowNum = 6;
	static const int MAX_bucketNum = 205000; 

	CMMSketch(int bNum, int rNum);
	bool insert(char* s, int num);
	int query(char* s, int opt);
	int middle(int* cnt, int num);
	unsigned short cnt[MAX_rowNum][MAX_bucketNum];
    int total_counter;
	BOBHash32 * hash;
};

CMMSketch::CMMSketch(int bNum = 204800, int rNum = 3){
	bucketNum = bNum;
	rowNum = rNum;
	hash = new BOBHash32[rNum + 1];
    total_counter = 0;
	for(int i = 0; i < rNum; i++) {
		hash[i].initialize(rand() % MAX_PRIME32);
		for(int j = 0; j < bNum; ++j)
			cnt[i][j] = 0;
	}
	hash[rNum].initialize(rand() % MAX_PRIME32);
}

bool CMMSketch::insert(char* in, int incNum){
	unsigned int inNum[rowNum + 10];
	for(int i = 0; i < rowNum; i++){
		inNum[i] = hash[i].run(in, 4) % bucketNum;
	}
	for(int i = 0; i < rowNum; i++){
		cnt[i][inNum[i]] += incNum;
	}
    total_counter += incNum;
}

int CMMSketch::middle(int* cnt, int num){
    int res = 0;
    for (int i = 0; i < num; i++){
        res += cnt[i];
    }
    return res / num;
}

int CMMSketch::query(char* in, int opt){
	unsigned int inNum[rowNum + 10];
	for(int i = 0; i < rowNum; i++){
		inNum[i] = hash[i].run(in, 4) % bucketNum;
	}

	assert((opt >= 0) && (opt <= 1));
	
	if (opt == 1){
		int res[rowNum + 10];
		for (int i = 0; i < rowNum; i++){
			res[i] = (int)cnt[i][inNum[i]] - (total_counter - cnt[i][inNum[i]]) / (bucketNum - 1);
		}
		return middle(res, rowNum);
	}
	else{
		int res = INF;
		for (int i = 0; i < rowNum; i++){
			res = min(res, (int)cnt[i][inNum[i]]);
		}
		return res;
	}
}
#endif