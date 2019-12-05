// test
#include <math.h>
#include "stdio.h"
#include "time.h"
#include <stdlib.h>
#include "marco.h"
#define eps 0.00001

int MLUCOMCosineOp(float *output, float *inputX, float *inputY, int m, int n);
int CPUCOMCosineOp(float *output, float *inputX, float *inputY)
{
  float sqX[N];
  float sqY[N];
  for (int i = 0; i < N; i++)
  {
    sqX[i] = 0.0;
    sqY[i] = 0.0;
    for (int j = 0; j < M; j++)
    {
      sqX[i] += *(inputX + j * N + i) * (*(inputX + j * N + i));
      sqY[i] += *(inputY + j * N + i) * (*(inputY + j * N + i));
    }
    sqX[i] = sqrt(sqX[i] + eps);
    sqY[i] = sqrt(sqY[i] + eps);
  }
  // printf("_________this is anwser____________\n");
  for (int i = 0; i < N; i++)
  {
    output[i] = 0.0;
    for (int j = 0; j < M; j++)
    {
      output[i] += *(inputX + j * N + i) * (*(inputY + j * N + i));
    }
    output[i] /= (sqX[i] * sqY[i]);
    // printf("output[%d]: %f\n", i, output[i]);
  }
  return 0;
}
int test(float *data1, float *data2)
{
  float answer[N];
  float answer_cpu[N];
  float cpu_sum = 0.0;
  float err = 0.0;
  MLUCOMCosineOp(answer, data1, data2, M, N);
  CPUCOMCosineOp(answer_cpu, data1, data2);
  // 打印测试结果
  printf("_________this is test____________\n");
  for (int i = 0; i < N; i++)
  {
    err += fabs(answer_cpu[i] - answer[i]);
    cpu_sum += fabs(answer_cpu[i]);
   // printf("the [%d] : mlu = %f , cpu = %f\n", i, answer[i], answer_cpu[i]);
  }
  cpu_sum += eps;
  printf("cpu sum :%f\n", cpu_sum);
  printf("Total error:%f, MAE:%f\n", err, err / LEN);
  printf("err rate = %0.4f%%\n", err * 100.0 / cpu_sum);
  return 0;
}
int main()
{
  printf("M: %d\n", M);
  printf("N: %d\n", N);
  printf("LEN: %d\n", LEN);
  float data1[LEN];
  float data2[LEN];
  srand((unsigned)time(NULL));
  printf("test1\n");
  for (int i = 0; i < LEN; i++)
  {
    if (i % 2 == 0)
    {
      data1[i] = 0;
      data2[i] = 1;
    }
    else
    {
      data1[i] = 1;
      data2[i] = 0;
    }
  }
  test(data1, data2);

  printf("\ntest2\n");
  for (int i = 0; i < LEN; i++)
  {
    data1[i] = 1;
    data2[i] = -1;
  }
  test(data1, data2);

  printf("\ntest3\n");
  for (int i = 0; i < LEN; i++)
  {
    data1[i] = (float)(rand() % 10) - 5;
    data2[i] = (float)(rand() % 10) - 5;
  }
  test(data1, data2);

  printf("test4\n");
  for (int i = 0; i < LEN; i++)
  {
    data1[i] = 1;
    data2[i] = -1;
  }
  for (int i = 0; i < M; i++)
  {
    data2[i * N] = 1;
  }
  test(data1, data2);

  printf("\ntest5\n");
  for (int i = 0; i < LEN; i++)
  {
    data1[i] = 0;
    data2[i] = 0;
  }
  test(data1, data2);

  printf("\ntest6\n");
  FILE *f_input_1 = fopen("./input1.txt", "r");
  FILE *f_input_2 = fopen("./input2.txt", "r");
  for (int i = 0; i < LEN; i++)
  {
    fscanf(f_input_1, "%f\n", &data1[i]);
    fscanf(f_input_2, "%f\n", &data2[i]);
  }

  test(data1, data2);

  return 0;
}
