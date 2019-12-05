// test
#include <math.h>
#include "stdio.h"
#include "time.h"
#include <stdlib.h>
#include "marco.h"
#include <algorithm>
#define DEBUG_PRINT 0

int MLUCOMTripletlossOp(float* output, float* inputA, float* inputP, float* intputN, int m, int n);
int cpu_tripletloss_test(float* answer,float* dataA, float* dataP, float* dataN){
	float margin=0.5;
	float dap[N];
	float dan[N];
	for(int i=0; i<N; i++){
		dap[i]=0.0;
		dan[i]=0.0;
		for(int j=0; j<M; j++){
			dap[i]+=fabs(*(dataA+N*j+i)-*(dataP+N*j+i));
			dan[i]+=fabs(*(dataA+N*j+i)-*(dataN+N*j+i));
		}
#if DEBUG_PRINT
		printf("dap[%d]=%f ",i, dap[i]);
		printf("dan[%d]=%f\n",i, dan[i]);
#endif
		answer[i]= (dap[i]-dan[i]+margin)>0? dap[i]-dan[i]+margin: 0;
	}
}

int test(float* dataA, float* dataP, float* dataN){
  float answer[N];
  float answer_cpu[N];
  float cpu_sum=0.0;
  float err=0.0;
  MLUCOMTripletlossOp(answer, dataA, dataP, dataN, M, N);
  cpu_tripletloss_test(answer_cpu, dataA, dataP, dataN);
  
  // 打印测试结果
  for(int i=0; i<N; i++){
#if DEBUG_PRINT
        printf("myas : %f ",answer[i]);
        printf("cpuas : %f \n",answer_cpu[i]);
#endif
	   err+=fabs(answer[i]-answer_cpu[i]);
	   cpu_sum+=fabs(answer_cpu[i]);
    }
  printf("Total error:%f, MAE:%f\n", err, err/LEN);
  printf("err rate = %0.4f%%\n", err*100.0/cpu_sum);
  
  printf("\n");
  return 0;

}
int main(){
  printf("M: %d\n",M);
  printf("N: %d\n",N);
  printf("LEN: %d\n",LEN);
  float data1[LEN];
  float data2[LEN];
  float data3[LEN];
  float data4[LEN];
  float dataA[LEN];
  float dataP[LEN];
  float dataN[LEN];
  FILE* f_input_a = fopen("./inputa.txt", "r");
  FILE* f_input_p = fopen("./inputp.txt", "r");
  FILE* f_input_n = fopen("./inputn.txt", "r");
  srand((unsigned)time(NULL));
  for (int i = 0; i < LEN; i++) {
    data1[i] = 1;
    data2[i] = -1;
    data3[i] = 2;
    data4[i] = 0;
  }
  printf("+++++++++++++++++test1++++++++++++++++++++\n");
  test(data1,data2,data3); 
  printf("+++++++++++++++++test2++++++++++++++++++++\n");
  test(data1,data2,data4); 
  printf("+++++++++++++++++test3++++++++++++++++++++\n");
  test(data3,data1,data1); 
  printf("+++++++++++++++++test4++++++++++++++++++++\n");
  test(data1,data1,data1); 
  for (int i = 0; i < LEN; i++) {
    data1[i] = (float)(rand()%10)-5;
    data2[i] = (float)(rand()%10)-5;
    data3[i] = (float)(rand()%10)-5;
    fscanf(f_input_a, "%f\n", &dataA[i]);
    fscanf(f_input_a, "%f\n", &dataP[i]);
    fscanf(f_input_a, "%f\n", &dataN[i]);
  }
  printf("+++++++++++++++++test5++++++++++++++++++++\n");
  test(data1,data2,data3); 
  // 0 err above, because all the data are interger
  printf("+++++++++++++++++test6++++++++++++++++++++\n");
  test(dataA,dataP,dataN); 
  return 0;
}
