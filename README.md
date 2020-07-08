# UTF: Unbiased Estimation of Top-K Frequent Items

This repository contains all the related code of our paper "UTF: Unbiased Estimation of Top-K Frequent Items". 

## Introduction

Finding top-K frequent items is a fundamental problem in data stream processing, and the unbiasedness of estimated frequency is an important property in this problem. However, to the best of our knowledge, no existing algorithm achieves unbiasedness of reported top-K items, which is the property we care in practice. In this paper, we propose the Unbiased Top-K Framework (UTF for short). Our key idea is to decouple the processes of finding top-K items and estimating item frequencies, so as to make them uncorrelated, achieving unbiasedness for reported top-K items. Then we propose an optimization technique, namely error freezing,to raise the accuracy of UTF. Our experimental results show that,compared with the state-of-the-art, UTF decreases its error (ARE) by up to 1.7 * 10^4 times (757 times in average) in finding top-K frequent items. 



## About This Repository

This repository contains all the related code for our experiments in the `src` directory. We have implemented four UTF applications for finding top-K items: UTF+RA, UTF+HG, UTF+Freq, UTF+SS. We also compare our results with Unbiased Space Saving (USS). Specifically:

`src/UTF_ra_topk.cpp`: Experiments of UTF with Randomized Admission Policy

`src/UTF_hg_topk.cpp`: Experiments of UTF with HeavyGuardian

`src/UTF_fq_topk.cpp`: Experiments of UTF with Frequent

`src/UTF_ss_topk.cpp`: Experiments of UTF with SpaceSaving

`src/USS.cpp`: Experiments Unbiased SpaceSaving

`src/archive/`: Implementations of Unbiased SpaceSaving Algorithm (with StreamSummary data structure)



## Requirements

- Ubuntu 16.04.6 LTS
- g++ >= 7.5.0



## Usage

### Datasets

We conduct experiments on four datasets: CAIDA [1], Webpage [2], Network [3], and Synthetic datasets. CAIDA, Webpage and Network datasets are open-sourced real-world datasets, while Synthetic datasets are generated by Zip-f distributions. You can download CAIDA, Webpage and Network datasets in their respective websites:

```
CAIDA: https://www.caida.org/data/overview/
Webpage: http://fimi.ua.ac.be/data/webdocs.dat.gz
Network: http://snap.stanford.edu/data/
```
We also upload and share CAIDA [1] Dataset anonymously: `https://1drv.ms/u/s!AsGW_raqQfI_atRwBCWPcUVKcHY?e=26uc7l`.

#### Dataset Setup

First, download the related datasets and put them into the `data/` folder.

- For CAIDA, Webpage, Network, and Synthetic datasets: we have implemented related functions for dataset parsing in `src/data.h`. Please read `src/data.h` carefully and rename the datasets you would like to test in order to match with the name defined in `src/data.h`.

- You can also test on your custom datasets. You just need to implement your own function for parsing your custom datasets in `src/data.h`



### How to Build

You can simply build all the programs for our experiment with the following commands:

```
cd src/
make
```


### How to Reproduce Our Experiments

Once successfully building all the programs, you need to create your own test cases to specify your desired experimental settings in a file (e.g., test_cases.txt). All the executables accept one parameter called `file_name` and read your constructed test cases in `file_name`. We have provided a sample file in `src/test_cases.txt`. The contents of this file and their corresponding explanations are as follows:

```
5 0 CAIDA_result.txt           # Number_of_test_cases;   Dataset_id: 0:CAIDA, 1:Webpage, 2:Net, 3:Synthetic;   output_file_name
100 55 1000 8             # test cases 1:   Memory (KB);   $M_{top-K}/M$;   value_of_K;   $lambda$:number_of_cells_in_each_bucket
200 55 1000 8
300 55 1000 8
400 55 1000 8
500 55 1000 8
```



After that, you should modify `src/run.sh`, and then run `src/run.sh`. All the programs should be expected to run automatically and produce the results in a file specified in `src/test_cases.txt`.


## References
[1] The caida anonymized 2016 internet traces. http://www.caida.org/data/overview/.

[2] Real-life transactional dataset. http://fimi.ua.ac.be/data/.

[3] The Network dataset Internet Traces. http://snap.stanford.edu/data/.
