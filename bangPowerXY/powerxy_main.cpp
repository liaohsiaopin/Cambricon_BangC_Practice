// test
#include <math.h>
#include <cmath>
#include "stdio.h"
#include "time.h"
#include <stdlib.h>
#include "marco.h"

int MLUCOMPowerXYOp(float* output, float* inputX, float* inputY, int m, int n);
int test(float* answer, float* cpu_answer, float* dataX, float* dataY){
  MLUCOMPowerXYOp(answer, dataX, dataY, M, N);
  // 打印测试结果
  float err=0.0;
  float cpu_sum=0.0;
#if 0
  float error = 0.0;
  printf("first 10 result:\n");
  for(int i=0; i<10; i++){
        printf("%f ",answer[i]);
    }
  printf("\n");

  for(int i=0; i<LEN; i++){
	  error+=fabs(pow(dataX[i],dataY[i])-answer[i]);
    }
  printf("Total error:%f, MAE:%f\n", error, error/LEN);
#endif
  for(int i = 0; i < LEN;++i)
  {
     err +=fabs(cpu_answer[i] - answer[i]) ;
     cpu_sum +=fabs(cpu_answer[i]);
  }
  //printf("err=%0.4f\t error+%0.4f\n", err, error);
  printf("MAE = %0.4f\n", err/LEN);
  printf("err rate = %0.4f%%\n", err*100.0/cpu_sum);
  return 0;

}
int main(){
  printf("M: %d\n",M);
  printf("N: %d\n",N);
  printf("LEN: %d\n",LEN);
  FILE* f_input_x = fopen("./pow_x.txt", "r");
  FILE* f_input_y = fopen("./pow_y.txt", "r");
  FILE* f_output_data = fopen("./pow_out_cpu.txt", "r");
  float input_x[LEN];
  float input_y[LEN];
  float input_2[LEN];
  float input_4[LEN];
  float output_cpu[LEN];
  float answer[LEN];
  srand((unsigned)time(NULL));
  for (int i = 0; i < LEN; i++) {
    fscanf(f_input_x, "%f\n", &input_x[i]);
    fscanf(f_input_y, "%f\n", &input_y[i]);
    fscanf(f_output_data, "%f\n", &output_cpu[i]);
	input_2[i]=2.0;
	input_4[i]=4.0;
  }
  test(answer,output_cpu,input_x,input_y); 
  test(answer,input_4,input_2,input_2); 
  return 0;
}
