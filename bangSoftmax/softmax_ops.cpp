#include <stdlib.h>
#include "cnrt.h"
#include "cnrt_data.h"
#include "stdio.h"
#ifdef __cplusplus
extern "C" {
#endif
void SoftmaxKernel(half* intput, half* output);
#ifdef __cplusplus
}
#endif
void SoftmaxKernel(half* intput, half* output);

int MLUCOMSoftmaxOp(float* output, float* input, int dims_a) {
  // regarding device stream
  cnrtInit(0);
  cnrtDev_t dev;
  cnrtGetDeviceHandle(&dev, 0);
  cnrtSetCurrentDevice(dev);
  cnrtQueue_t queue;
  cnrtCreateQueue(&queue);
  cnrtDim3_t dim;
  dim.x = 1;
  dim.y = 1;
  dim.z = 1;
  cnrtFunctionType_t c = CNRT_FUNC_TYPE_BLOCK;

  // prepare data
  output[0] = 0;
  half* input_half = (half*)malloc(dims_a * sizeof(half));
  half* output_half = (half*)malloc(dims_a * sizeof(half));
  for (int i = 0; i < dims_a; i++) {
    cnrtConvertFloatToHalf(&input_half[i], input[i]);
    // printf("%f\n", input[i]);
  }
  for (int i = 0; i < dims_a; i++) {
    cnrtConvertFloatToHalf(&output_half[i], output[i]);
    // printf("%f\n", input[i]);
  }

  half *mlu_input, *mlu_output;
  if (CNRT_RET_SUCCESS !=
      cnrtMalloc((void**)&mlu_input, dims_a * sizeof(half))) {
    printf("cnrtMalloc FAILED!\n");
    exit(-1);
  }
  if (CNRT_RET_SUCCESS != 
      cnrtMalloc((void**)&mlu_output, dims_a * sizeof(half))) {
    printf("cnrtMalloc FAILED!\n");
    exit(-1);
  }

  if (CNRT_RET_SUCCESS != cnrtMemcpy(mlu_input, input_half,
                                     dims_a * sizeof(half),
                                     CNRT_MEM_TRANS_DIR_HOST2DEV)) {
    printf("cnrtMemcpy FAILED!\n");
    exit(-1);
  }
  // cnrtMemcpy(mlu_output, output_half, sizeof(half),
  // CNRT_MEM_TRANS_DIR_HOST2DEV)

  // kernel parameters
  cnrtKernelParamsBuffer_t params;
  cnrtGetKernelParamsBuffer(&params);
  cnrtKernelParamsBufferAddParam(params, &mlu_input, sizeof(half*));
  cnrtKernelParamsBufferAddParam(params, &mlu_output, sizeof(half*));
  // ++
  cnrtNotifier_t notifier_start;
  cnrtNotifier_t notifier_end;
  cnrtCreateNotifier(&notifier_start);
  cnrtCreateNotifier(&notifier_end);
  float timeTotal = 0.0;
  struct timeval start;
  struct timeval end;
  //gettimeofday(&start, NULL);
  cnrtPlaceNotifier(notifier_start,queue);
  if (CNRT_RET_SUCCESS !=
      cnrtInvokeKernel_V2((void*)&SoftmaxKernel, dim, params, c, queue)) {
    printf("cnrtInvokeKernel FAILED!\n");
    exit(-1);
  }
  cnrtPlaceNotifier(notifier_end,queue);
  if (CNRT_RET_SUCCESS != cnrtSyncQueue(queue)) {
    printf("cnrtSyncQueue FAILED!\n");
    exit(-1);
  }
 // gettimeofday(&end, NULL);
  cnrtNotifierDuration(notifier_start, notifier_end, &timeTotal);
  printf("Hardware Total Time: %.3f ms\n",timeTotal/1000.0);
  cnrtDestroyNotifier(&notifier_start);
  cnrtDestroyNotifier(&notifier_end);

  // get output data
  if (CNRT_RET_SUCCESS != 
    cnrtMemcpy(output_half, mlu_output, dims_a*sizeof(half),
                                     CNRT_MEM_TRANS_DIR_DEV2HOST)) {
    printf("cnrtMemcpy FAILED!\n");
    exit(-1);
  }

  for (int i = 0; i < dims_a; i++) {
    cnrtConvertHalfToFloat(output+i, output_half[i]);
    // printf("%f\n", input[i]);
  }

  // free data
  if (CNRT_RET_SUCCESS != cnrtFree(mlu_input)) {
    printf("cnrtFree FAILED!\n");
    exit(-1);
  }
  if (CNRT_RET_SUCCESS != cnrtFree(mlu_output)) {
    printf("cnrtFree FAILED!\n");
    exit(-1);
  }

  if (CNRT_RET_SUCCESS != cnrtDestroyQueue(queue)) {
    printf("cnrtDestroyQueue FAILED!\n");
    exit(-1);
  }
  if (CNRT_RET_SUCCESS != cnrtDestroyKernelParamsBuffer(params)) {
    printf("cnrtDestroyKernelParamsBuffer failed!\n");
    return -1;
  }
  cnrtDestroy();
  free(input_half);
  free(output_half);
  return 0;
}
