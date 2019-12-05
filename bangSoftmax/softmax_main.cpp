// test
#include <math.h>
#include "stdio.h"
#include "time.h"
#include <stdlib.h>
#include <cmath>
#define INPUT_SIZE 20
#define INPUT_NUM  256
#define LEN (INPUT_SIZE*INPUT_NUM)
#define FLOAT_SMALL -3000.0
#define DEBUG_P 0
int MLUCOMSoftmaxOp(float* output, float* input, int len);

int CPUCOMSoftmaxOp(float* input){
	for(int i=0; i<INPUT_NUM; i++){
		float large = FLOAT_SMALL;
		float sum = 0.0;
		for(int j =0; j<INPUT_SIZE;j++){
			if(large<*(input+j*INPUT_NUM+i))
				large=*(input+j*INPUT_NUM+i);
#if DEBUG_P
			if(i==20){
				printf("large: %f\n", large);
			}
#endif
		}
		for(int j =0; j<INPUT_SIZE;j++){
			*(input+j*INPUT_NUM+i)-=large;
#if DEBUG_P
			if(i==20){
				printf("after sub: %f\n", *(input+j*INPUT_NUM+i));
			}
#endif
		}
		for(int j =0; j<INPUT_SIZE;j++){
			*(input+j*INPUT_NUM+i)=exp(*(input+j*INPUT_NUM+i));
#if DEBUG_P
			if(i==20){
				printf("after exp: %f\n", *(input+j*INPUT_NUM+i));
			}
#endif
			sum+=*(input+j*INPUT_NUM+i);
#if DEBUG_P
			if(i==20){
				printf("sum: %f\n", sum);
			}
#endif
		}
		float a=0.0;
		for(int j =0; j<INPUT_SIZE;j++){
			*(input+j*INPUT_NUM+i)/=sum;
#if DEBUG_P
			if(i==20){
				printf("after everything: %f\n", *(input+j*INPUT_NUM+i));
				a+= *(input+j*INPUT_NUM+i);
				printf("sum: %f\n", a);
			}
#endif
		}
	}
	return 0;
}

int test(float* answer, float* data){
  float cpu_sum=0.0;
  float err=0.0;
  MLUCOMSoftmaxOp(answer, data, LEN);
  CPUCOMSoftmaxOp(data);
  // 打印测试结果
#if 0
  for(int i=0; i<INPUT_NUM; i++){
    float sum = 0.0;
    printf("value[%d]:\n", i);
    for(int j=0; j<INPUT_SIZE; j++){ 
        printf("mlu_ %f \n",answer[j*INPUT_NUM+i]);
        printf("cpu_ %f \n",data[j*INPUT_NUM+i]);
        sum+=answer[j*INPUT_NUM+i];
    }
    printf("\n");
    printf("sum: %f\n\n", sum );
    if (fabs(sum - 1.0) > 0.01) printf("FAILED\n");
	}
#endif
  for(int i=0; i<LEN; i++){
	err+=fabs(answer[i]-data[i]);
	cpu_sum+=fabs(data[i]);
  }
  printf("Total error:%f, MAE:%f\n", err, err/LEN);
  printf("err rate = %0.4f%%\n", err*100.0/cpu_sum);

}

int main() {
  printf("INPUT_SIZE: %d\n",INPUT_SIZE);
  printf("INPUT_NUM: %d\n",INPUT_NUM);
  printf("LEN: %d\n",LEN);
  float data[LEN];
  float file_data[LEN];
  FILE* f_input = fopen("./inputsoftmax.txt", "r");
  srand((unsigned)time(NULL));
  for (int i = 0; i < LEN; i++) {
    //data[i] = 1;
    data[i] = (float)(rand()%10);
    fscanf(f_input, "%f\n", &file_data[i]);
  }
  float answer[LEN];
  test(answer,data);
  test(answer,file_data);
  return 0;
}
