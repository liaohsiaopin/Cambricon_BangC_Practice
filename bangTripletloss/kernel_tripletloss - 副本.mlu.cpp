#include "mlu.h"
#include "marco.h"
#define EPS 0.0001
#define MARGIN 0.5
#define ONELINE 256

__mlu_func__ void __bang_abs(half *src)
{
    __bang_active_abs(src,src,ONELINE);
}

__mlu_entry__ void TripletlossKernel(half *inputA, half *inputP, half *inputN, half *output)
{

    __nram__ half inputA_nram[N];
    __nram__ half inputP_nram[N];
    __nram__ half inputN_nram[N];
    __nram__ half tempa_nram[N];
    __nram__ half tempb_nram[N];
    __nram__ half adda_nram[N];
    __nram__ half addb_nram[N];
    __nram__ half const_nram[N];

    __nramset_half(adda_nram, ONELINE, 0.0);
    __nramset_half(addb_nram, ONELINE, 0.0);
    __nramset_half(const_nram, ONELINE, MARGIN);

    for (int32_t i = 0; i < M; i++)
    {
      
        __memcpy(inputA_nram , inputA + i * N, N * sizeof(half), GDRAM2NRAM);
        __memcpy(inputP_nram , inputP + i * N, N * sizeof(half), GDRAM2NRAM);
        __memcpy(inputN_nram , inputN + i * N, N * sizeof(half), GDRAM2NRAM);

        // __bang_mul_const(calX_nram, calX_nram, 0.005, ONELINE);

        __bang_sub(tempa_nram, inputA_nram, inputP_nram, ONELINE); // A-P
        __bang_abs(tempa_nram);  //  | A - P |
        __bang_add(adda_nram, adda_nram, tempa_nram, ONELINE); // adda = sum (| A - P |)

        __bang_sub(tempb_nram, inputA_nram, inputN_nram, ONELINE); // A-N
        __bang_abs(tempb_nram);  //  | A - N |
        __bang_add(addb_nram, addb_nram, tempb_nram, ONELINE); // addb = sum (| A - N |)

    }

    __bang_sub(tempa_nram, adda_nram,addb_nram, ONELINE); // tempa = adda - addb 
    __bang_add(tempb_nram, tempa_nram, const_nram, ONELINE); // tempa += margin

    // if <0 change it to 0
     for (int32_t i = 0; i < N; i++)
    {
        if (tempb_nram[i]<0)
        {
            tempb_nram[i] = 0 ;
        }
    }

    __memcpy(output , tempb_nram, ONELINE * sizeof(half), NRAM2GDRAM);
    
}