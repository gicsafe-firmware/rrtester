/* AUTOGENERATED FILE. DO NOT EDIT. */
#include <string.h>
#include <stdlib.h>
#include <setjmp.h>
#include "unity.h"
#include "cmock.h"
#include "Mock_adconv.h"

static const char* CMockString_ADConv_getSample = "ADConv_getSample";
static const char* CMockString_channel = "channel";

typedef struct _CMOCK_ADConv_getSample_CALL_INSTANCE
{
  UNITY_LINE_TYPE LineNumber;
  SampleValue ReturnVal;
  int CallOrder;
  int Expected_channel;
  int IgnoreArg_channel;

} CMOCK_ADConv_getSample_CALL_INSTANCE;

static struct Mock_adconvInstance
{
  int ADConv_getSample_IgnoreBool;
  SampleValue ADConv_getSample_FinalReturn;
  CMOCK_ADConv_getSample_CALLBACK ADConv_getSample_CallbackFunctionPointer;
  int ADConv_getSample_CallbackCalls;
  CMOCK_MEM_INDEX_TYPE ADConv_getSample_CallInstance;
} Mock;

extern jmp_buf AbortFrame;
extern int GlobalExpectCount;
extern int GlobalVerifyOrder;

void Mock_adconv_Verify(void)
{
  UNITY_LINE_TYPE cmock_line = TEST_LINE_NUM;
  if (Mock.ADConv_getSample_IgnoreBool)
    Mock.ADConv_getSample_CallInstance = CMOCK_GUTS_NONE;
  UNITY_SET_DETAIL(CMockString_ADConv_getSample);
  UNITY_TEST_ASSERT(CMOCK_GUTS_NONE == Mock.ADConv_getSample_CallInstance, cmock_line, CMockStringCalledLess);
  if (Mock.ADConv_getSample_CallbackFunctionPointer != NULL)
    Mock.ADConv_getSample_CallInstance = CMOCK_GUTS_NONE;
}

void Mock_adconv_Init(void)
{
  Mock_adconv_Destroy();
}

void Mock_adconv_Destroy(void)
{
  CMock_Guts_MemFreeAll();
  memset(&Mock, 0, sizeof(Mock));
  Mock.ADConv_getSample_CallbackFunctionPointer = NULL;
  Mock.ADConv_getSample_CallbackCalls = 0;
  GlobalExpectCount = 0;
  GlobalVerifyOrder = 0;
}

SampleValue ADConv_getSample(int channel)
{
  UNITY_LINE_TYPE cmock_line = TEST_LINE_NUM;
  CMOCK_ADConv_getSample_CALL_INSTANCE* cmock_call_instance;
  UNITY_SET_DETAIL(CMockString_ADConv_getSample);
  cmock_call_instance = (CMOCK_ADConv_getSample_CALL_INSTANCE*)CMock_Guts_GetAddressFor(Mock.ADConv_getSample_CallInstance);
  Mock.ADConv_getSample_CallInstance = CMock_Guts_MemNext(Mock.ADConv_getSample_CallInstance);
  if (Mock.ADConv_getSample_IgnoreBool)
  {
    UNITY_CLR_DETAILS();
    if (cmock_call_instance == NULL)
      return Mock.ADConv_getSample_FinalReturn;
    memcpy(&Mock.ADConv_getSample_FinalReturn, &cmock_call_instance->ReturnVal, sizeof(SampleValue));
    return cmock_call_instance->ReturnVal;
  }
  UNITY_TEST_ASSERT_NOT_NULL(cmock_call_instance, cmock_line, CMockStringCalledMore);
  cmock_line = cmock_call_instance->LineNumber;
  if (cmock_call_instance->CallOrder > ++GlobalVerifyOrder)
    UNITY_TEST_FAIL(cmock_line, CMockStringCalledEarly);
  if (cmock_call_instance->CallOrder < GlobalVerifyOrder)
    UNITY_TEST_FAIL(cmock_line, CMockStringCalledLate);
  if (!cmock_call_instance->IgnoreArg_channel)
  {
    UNITY_SET_DETAILS(CMockString_ADConv_getSample,CMockString_channel);
    UNITY_TEST_ASSERT_EQUAL_INT(cmock_call_instance->Expected_channel, channel, cmock_line, CMockStringMismatch);
  }
  if (Mock.ADConv_getSample_CallbackFunctionPointer != NULL)
  {
    cmock_call_instance->ReturnVal = Mock.ADConv_getSample_CallbackFunctionPointer(channel, Mock.ADConv_getSample_CallbackCalls++);
  }
  UNITY_CLR_DETAILS();
  return cmock_call_instance->ReturnVal;
}

void CMockExpectParameters_ADConv_getSample(CMOCK_ADConv_getSample_CALL_INSTANCE* cmock_call_instance, int channel)
{
  cmock_call_instance->Expected_channel = channel;
  cmock_call_instance->IgnoreArg_channel = 0;
}

void ADConv_getSample_CMockIgnoreAndReturn(UNITY_LINE_TYPE cmock_line, SampleValue cmock_to_return)
{
  CMOCK_MEM_INDEX_TYPE cmock_guts_index = CMock_Guts_MemNew(sizeof(CMOCK_ADConv_getSample_CALL_INSTANCE));
  CMOCK_ADConv_getSample_CALL_INSTANCE* cmock_call_instance = (CMOCK_ADConv_getSample_CALL_INSTANCE*)CMock_Guts_GetAddressFor(cmock_guts_index);
  UNITY_TEST_ASSERT_NOT_NULL(cmock_call_instance, cmock_line, CMockStringOutOfMemory);
  memset(cmock_call_instance, 0, sizeof(*cmock_call_instance));
  Mock.ADConv_getSample_CallInstance = CMock_Guts_MemChain(Mock.ADConv_getSample_CallInstance, cmock_guts_index);
  Mock.ADConv_getSample_IgnoreBool = (int)0;
  cmock_call_instance->LineNumber = cmock_line;
  cmock_call_instance->ReturnVal = cmock_to_return;
  Mock.ADConv_getSample_IgnoreBool = (int)1;
}

void ADConv_getSample_CMockExpectAndReturn(UNITY_LINE_TYPE cmock_line, int channel, SampleValue cmock_to_return)
{
  CMOCK_MEM_INDEX_TYPE cmock_guts_index = CMock_Guts_MemNew(sizeof(CMOCK_ADConv_getSample_CALL_INSTANCE));
  CMOCK_ADConv_getSample_CALL_INSTANCE* cmock_call_instance = (CMOCK_ADConv_getSample_CALL_INSTANCE*)CMock_Guts_GetAddressFor(cmock_guts_index);
  UNITY_TEST_ASSERT_NOT_NULL(cmock_call_instance, cmock_line, CMockStringOutOfMemory);
  memset(cmock_call_instance, 0, sizeof(*cmock_call_instance));
  Mock.ADConv_getSample_CallInstance = CMock_Guts_MemChain(Mock.ADConv_getSample_CallInstance, cmock_guts_index);
  Mock.ADConv_getSample_IgnoreBool = (int)0;
  cmock_call_instance->LineNumber = cmock_line;
  cmock_call_instance->CallOrder = ++GlobalExpectCount;
  CMockExpectParameters_ADConv_getSample(cmock_call_instance, channel);
  memcpy(&cmock_call_instance->ReturnVal, &cmock_to_return, sizeof(SampleValue));
  UNITY_CLR_DETAILS();
}

void ADConv_getSample_StubWithCallback(CMOCK_ADConv_getSample_CALLBACK Callback)
{
  Mock.ADConv_getSample_IgnoreBool = (int)0;
  Mock.ADConv_getSample_CallbackFunctionPointer = Callback;
}

void ADConv_getSample_CMockIgnoreArg_channel(UNITY_LINE_TYPE cmock_line)
{
  CMOCK_ADConv_getSample_CALL_INSTANCE* cmock_call_instance = (CMOCK_ADConv_getSample_CALL_INSTANCE*)CMock_Guts_GetAddressFor(CMock_Guts_MemEndOfChain(Mock.ADConv_getSample_CallInstance));
  UNITY_TEST_ASSERT_NOT_NULL(cmock_call_instance, cmock_line, CMockStringIgnPreExp);
  cmock_call_instance->IgnoreArg_channel = 1;
}
