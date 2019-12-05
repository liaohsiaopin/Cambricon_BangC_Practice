#include <stdlib.h>
#include "cnrt.h"
#include "cnrt_data.h"
#include "stdio.h"
#ifdef __cplusplus
extern "C" {
#endif
void PowerXYKernel(half* inputX, half* inputY, half* output);
#ifdef __cplusplus
}
#endif
void PowerXYKernel(half* inputX, half* inputY, half* output);

int MLUCOMPowerXYOp(float* output, float* inputX, float* inputY, int m, int n) {
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
  half* inputX_half = (half*)malloc(m*n * sizeof(half));
  half* inputY_half = (half*)malloc(m*n * sizeof(half));
  half* output_half = (half*)malloc(m*n * sizeof(half));
  for (int i = 0; i < m*n; i++) {
    cnrtConvertFloatToHalf(&inputX_half[i], inputX[i]);
    cnrtConvertFloatToHalf(&inputY_half[i], inputY[i]);
    // printf("%f\n", input[i]);
  }
  for (int i = 0; i < m*n; i++) {
    cnrtConvertFloatToHalf(&output_half[i], output[i]);
    // printf("%f\n", input[i]);
  }

  half *mlu_inputX, *mlu_inputY, *mlu_output;
  if (CNRT_RET_SUCCESS !=
      cnrtMalloc((void**)&mlu_inputX, m*n * sizeof(half))) {
    printf("cnrtMalloc FAILED!\n");
    exit(-1);
  }
  if (CNRT_RET_SUCCESS !=
      cnrtMalloc((void**)&mlu_inputY, m*n * sizeof(half))) {
    printf("cnrtMalloc FAILED!\n");
    exit(-1);
  }
  if (CNRT_RET_SUCCESS != 
      cnrtMalloc((void**)&mlu_output, m*n * sizeof(half))) {
    printf("cnrtMalloc FAILED!\n");
    exit(-1);
  }

  if (CNRT_RET_SUCCESS != cnrtMemcpy(mlu_inputX, inputX_half,
                                     m*n * sizeof(half),
                                     CNRT_MEM_TRANS_DIR_HOST2DEV)) {
    printf("cnrtMemcpy FAILED!\n");
    exit(-1);
  }
  if (CNRT_RET_SUCCESS != cnrtMemcpy(mlu_inputY, inputY_half,
                                     m*n * sizeof(half),
                                     CNRT_MEM_TRANS_DIR_HOST2DEV)) {
    printf("cnrtMemcpy FAILED!\n");
    exit(-1);
  }
//printf("-----------cpu0-----------\n");
  // kernel parameters
  cnrtKernelParamsBuffer_t params;
  cnrtGetKernelParamsBuffer(&params);
  cnrtKernelParamsBufferAddParam(params, &mlu_inputX, sizeof(half*));
  cnrtKernelParamsBufferAddParam(params, &mlu_inputY, sizeof(half*));
  cnrtKernelParamsBufferAddParam(params, &mlu_output, sizeof(half*));
//printf("-----------cpu1-----------\n");
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
      cnrtInvokeKernel_V2((void*)&PowerXYKernel, dim, params, c, queue)) {
    printf("cnrtInvokeKernel FAILED!\n");
    exit(-1);
  }
  cnrtPlaceNotifier(notifier_end,queue);
  if (CNRT_RET_SUCCESS != cnrtSyncQueue(queue)) {
    printf("cnrtSyncQueue FAILED!\n");
    exit(-1);
  }
//printf("-----------cpu2-----------\n");
 // gettimeofday(&end, NULL);
  cnrtNotifierDuration(notifier_start, notifier_end, &timeTotal);
  printf("Hardware Total Time: %.3f ms\n",timeTotal/1000.0);
  cnrtDestroyNotifier(&notifier_start);
  cnrtDestroyNotifier(&notifier_end);
//printf("--debug--\n");
  // get output data
  if (CNRT_RET_SUCCESS != 
    cnrtMemcpy(output_half, mlu_output, m*n*sizeof(half),
                                     CNRT_MEM_TRANS_DIR_DEV2HOST)) {
    printf("cnrtMemcpy FAILED!\n");
    exit(-1);
  }
//printf("-----------cpu3-----------\n");

  for (int i = 0; i < m*n; i++) {
    cnrtConvertHalfToFloat(output+i, output_half[i]);
    // printf("%f\n", input[i]);
  }

//printf("--debug-2-\n");
  // free data
  if (CNRT_RET_SUCCESS != cnrtFree(mlu_inputX)) {
    printf("cnrtFree FAILED!\n");
    exit(-1);
  }
  
  if (CNRT_RET_SUCCESS != cnrtFree(mlu_inputY)) {
    printf("cnrtFree FAILED!\n");
    exit(-1);
  }
  if (CNRT_RET_SUCCESS != cnrtFree(mlu_output)) {
    printf("cnrtFree FAILED!\n");
    exit(-1);
  }

//printf("--debug-3-\n");
  if (CNRT_RET_SUCCESS != cnrtDestroyQueue(queue)) {
    printf("cnrtDestroyQueue FAILED!\n");
    exit(-1);
  }
  if (CNRT_RET_SUCCESS != cnrtDestroyKernelParamsBuffer(params)) {
    printf("cnrtDestroyKernelParamsBuffer failed!\n");
    return -1;
  }
  cnrtDestroy();
  free(inputX_half);
  free(inputY_half);
  free(output_half);
  return 0;
}
