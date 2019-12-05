#include "mlu.h"
#include "marco.h"
#define EPS 0.00001
#define BETA  0.5  //0.4955
#define DEBUG_PRINT 1
#define M 128
#define N 256

__mlu_entry__ void bnKernel(half *input, half *output)
{

    __nram__ half input_nram[LEN];
    __nram__ half tans_nram[LEN];
    __nram__ half temp1_nram[M];
    __nram__ half avg_nram[M];
    __nram__ half var_nram[M];
    __nram__ half sta_nram[LEN];
    __nram__ half esp_nram[M];
    __nram__ half beta_nram[LEN];
    __nram__ half temp2_nram[M];
    __nram__ half result_nram[LEN];

    __nramset_half(temp1_nram, M, 0.0);
    __nramset_half(temp2_nram, M, 0.0);
    __nramset_half(result_nram, LEN, 0.0);
    __nramset_half(esp_nram, M, EPS);
    __nramset_half(beta_nram, LEN, BETA);

    __nram__ half recipN = 1 / (half)N; // 1/N

    // INPUT TO NRAN
    __memcpy(input_nram, input, LEN * sizeof(half), GDRAM2NRAM);

    // tans
    __bang_transpose(tans_nram, input_nram, M, N);

    // sum(x)
    for (int32_t i = 0; i < N; i++)
    {
        __bang_add(temp1_nram, tans_nram + i * M, temp1_nram, M);
    }
    // avg = sum(x) / N
    __bang_mul_const(avg_nram, temp1_nram, recipN , M);

    //variance
    // sum((x-avg)^2)
    for (int32_t i = 0; i < N; i++)
    {
        __bang_sub(temp1_nram, tans_nram + i * M, avg_nram, M); // X  - AVG
        __bang_mul(temp1_nram, temp1_nram, temp1_nram, M);      // (X-AVG)^2
        __bang_add(temp2_nram, temp1_nram, temp2_nram, M);      // temp2 = SUM((X-AVG)^2)
    }
    // var = temp2 / N
    __bang_mul_const(var_nram, temp2_nram, REN, M);

    // Standardization x

    __bang_add(temp2_nram, var_nram, esp_nram, M);  // temp2 = var + esp
    __bang_active_sqrt(temp2_nram, var_nram, M);    // temp2 = sqrt(var + esp)
    __bang_active_recip(temp2_nram, temp2_nram, M); // temp2 = 1/temp2 = 1/sqrt(var + esp)

    for (int32_t i = 0; i < N; i++)
    {
        __bang_sub(temp1_nram, tans_nram + i * M, avg_nram, M);  // temp1 = X  - AVG
        __bang_mul(sta_nram + i * M, temp1_nram, temp2_nram, M); // sta = temp1 * temp2 = X  - AVG / sqrt(var + esp)
    }

    // result = 2.0 * staX + beta  
    __bang_mul_const(result_nram, sta_nram, 1.957, LEN); // 2.0
    __bang_add(result_nram, result_nram, beta_nram, LEN); // RESULT + =beta

    // tans return
    __bang_transpose(tans_nram, result_nram, N, M);

    //OUTPUT TO GD
    __memcpy(output, tans_nram, LEN * sizeof(half), NRAM2GDRAM);


}

