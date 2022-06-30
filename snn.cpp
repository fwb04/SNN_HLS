#include "snn.h"
#include "fstream"
using namespace std;


uint1_t act_fun(float24_t mem){
    
    if(mem > (float24_t)0.4) return 1;
    else return 0;
}


// full-connected 2*32
void fc1(uint1_t ecg1, uint1_t ecg2, float24_t *mem1, uint1_t *spike1, const float24_t *weight){
    Fc1Loop:
    for(int i=0; i<32; i++){
        mem1[i] = mem1[i]*(float24_t)0.95*(float24_t)(1-spike1[i]) + ecg1*weight[0+i*2] + ecg2*weight[1+i*2] + weight[64+i];
        spike1[i] = act_fun(mem1[i]);
    }
}

// full-connected 32*32
void fc2(uint1_t spike_in[32], float24_t *mem2, uint1_t *spike2, const float24_t *weight){
	Fc2Loop:
    for(int i=0; i<32; i++){
        mem2[i] = mem2[i]*(float24_t)0.95*(float24_t)(1-spike2[i]);
        MulSumLoop1:
        for(int j=0; j<32; j++){
            mem2[i] += spike_in[j]*weight[96+j+i*32];
        }
        mem2[i] += weight[1120+i];
        spike2[i] = act_fun(mem2[i]);
    }
}

// full-connected 32*32
void fc3(uint1_t spike_in[32], float24_t *mem3, uint1_t *spike3, const float24_t *weight){
	Fc3Loop:
    for(int i=0; i<32; i++){
        mem3[i] = mem3[i] * (float24_t)0.95 * (float24_t)(1-spike3[i]);
        MulSumLoop2:
        for(int j=0; j<32; j++){
            mem3[i] += spike_in[j]*weight[1152+j+i*32];
        }
        mem3[i] += weight[2176+i];
        spike3[i] = act_fun(mem3[i]);
    }
}

// full-connected 32*4
void fc4(uint1_t spike_in[32], float24_t *mem_out, uint1_t *spike_out, const float24_t *weight){
	Fc4Loop:
    for(int i=0; i<4; i++){
        mem_out[i] = mem_out[i]*(float24_t)0.95* (float24_t)(1-spike_out[i]);
        MulSumLoop3:
        for(int j=0; j<32; j++){
            mem_out[i] += spike_in[j]*weight[2208+j+i*32];
        }
        mem_out[i] += weight[2336+i];
        spike_out[i] = act_fun(mem_out[i]);
    }
}

void SNN_infer(uint1_t ecg[260], int *result){

    const float24_t weight[2340] = {
        #include "weights.h"
    };
    
    int i=0;
    static float24_t mem1[32];
    static uint1_t spike1[32];
    static float24_t mem2[32];
    static uint1_t spike2[32];
    static float24_t mem3[32];
    static uint1_t spike3[32];
    static float24_t mem_out[4];
    static uint1_t spike_out[4];
    static ap_uint<8> spike_sum[4];
    
    TimeLoop:
    for(int time=0; time<130; time++){
        fc1(ecg[time], ecg[time+130], mem1, spike1, weight);
        fc2(spike1, mem2, spike2, weight);
        fc3(spike2, mem3, spike3, weight);
        fc4(spike3, mem_out, spike_out, weight);
        SumLoop:
        for(i=0; i<4; i++){
            spike_sum[i] += spike_out[i];
        }
    }
    ap_uint<8> max=0;
    FindMaxLoop:
    for(i=0; i<4; i++){
    	cout << spike_sum[i]/130.0 << ' ';
        if(max<spike_sum[i]){
            max = spike_sum[i];
            *result = i;
        }
    }
}


