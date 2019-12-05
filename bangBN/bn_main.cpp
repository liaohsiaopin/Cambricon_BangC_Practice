// test
#include <math.h>
#include "stdio.h"
#include "time.h"
#include <stdlib.h>
#include "marco.h"

int MLUCOMbnOp(float *output, float *input, int len);
int CPUCOMbnOp(float *input)
{
  float gamma = 2.0;
  float beta = 0.5;
  float eps = 0.0001;
  float mean[M];
  float var[M];
  //mean
  for (int i = 0; i < M; i++)
  {
    mean[i] = 0.0;
    for (int j = 0; j < N; j++)
    {
      mean[i] += *(input + i * N + j);
    }
    mean[i] /= N;
    //		printf("mean[%d]:%f\n", i,mean[i]);
  }
  //variance
  for (int i = 0; i < M; i++)
  {
    var[i] = 0.0;
    for (int j = 0; j < N; j++)
    {
      var[i] += (*(input + i * N + j) - mean[i]) * (*(input + i * N + j) - mean[i]);
    }
    var[i] /= N;
    //		printf("var[%d]:%f\n", i, var[i]);
  }
  //varicane after sqrt
  for (int i = 0; i < M; i++)
  {
    var[i] = sqrt(var[i] + eps);
  }
  for (int i = 0; i < M; i++)
  {
    for (int j = 0; j < N; j++)
    {
      //x hat
      *(input + N * i + j) = (*(input + N * i + j) - mean[i]) / var[i];
      //y
      *(input + N * i + j) = *(input + N * i + j) * gamma + beta;
    }
  }

#if 0
  for(int i=0; i<LEN; i++){
    if(i%N==0)
    printf("cpuval[%d]: %f\n", i,input[i] );
  }
#endif
}

int test(float *inputbn)
{
  float answer[LEN];
  float cpu_sum = 0.0;
  float err = 0.0;
  MLUCOMbnOp(answer, inputbn, LEN);
  CPUCOMbnOp(inputbn);
  // 打印测试结果
  for (int i = 0; i < LEN; i++)
  {
    err += fabs(answer[i] - inputbn[i]);
    cpu_sum += fabs(inputbn[i]);
#if 1
    if (i % N == 0)
    {
      // printf("val[%d]: %f\n", i, answer[i]);
      // printf("the[%d] : mlu = %f, ans = %f \n", i, answer[i], inputbn[i]);
    }
#endif
  }
  printf("Total error:%f, MAE:%f\n", err, err / LEN);
  printf("err rate = %0.4f%%\n", err * 100.0 / cpu_sum);
}

int main()
{
  printf("M: %d\n", M);
  printf("N: %d\n", N);
  printf("LEN: %d\n", LEN);
  float data[LEN];
  float data1[LEN];
  float file_data[LEN];
  FILE *f_input = fopen("./inputbn1.txt", "r");
  srand((unsigned)time(NULL));
  for (int i = 0; i < LEN; i++)
  {
    data1[i] = 1;
    data[i] = (float)(rand() % 10);
    fscanf(f_input, "%f\n", &file_data[i]);
  }
  printf("++++++++++++++++test1+++++++++++++++++++");
  test(data);
  printf("++++++++++++++++test2+++++++++++++++++++");
  test(data1);
  printf("++++++++++++++++testfile+++++++++++++++++++");
  test(file_data);
  return 0;
}
