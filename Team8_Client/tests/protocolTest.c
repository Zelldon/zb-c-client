/*
 * File:   protocolTest.c
 * Author: Christopher Zell <christopher.zell@camunda.com>
 *
 * Created on Oct 11, 2016, 4:06:41 PM
 */

#include <stdio.h>
#include <stdlib.h>
#include <CUnit/Basic.h>
#include "../protocol.h"

/*
 * CUnit Test Suite
 */

int init_suite(void) {
  return 0;
}

int clean_suite(void) {
  return 0;
}

void testCreateTransportProtocol() {
  //given
  int16_t templateId = CREATE_TASK_REQUEST;

  //when
  struct TransportProtocol* transportProtocol = createTransportProtocol(templateId);

  //then
  CU_ASSERT_PTR_NOT_NULL(transportProtocol);
  CU_ASSERT_EQUAL(0, transportProtocol->protocolId);
  CU_ASSERT_EQUAL(2562132L, transportProtocol->connectionId);
  CU_ASSERT_EQUAL(2551312L, transportProtocol->requestId);
  CU_ASSERT_EQUAL(0, transportProtocol->blockLength);
  CU_ASSERT_EQUAL(CREATE_TASK_REQUEST, transportProtocol->templateId);
  CU_ASSERT_EQUAL(1, transportProtocol->schemaId);
  CU_ASSERT_EQUAL(1, transportProtocol->version);
  CU_ASSERT_EQUAL(0, transportProtocol->resourceId);
  CU_ASSERT_EQUAL(-2, transportProtocol->shardId);

  //clean up
  freeTransportProtocol(transportProtocol);
}

//
//void testCreateMessage() {
//  TransportProtocol* transportProtocol;
//  int32_t len;
//  Message* result = createMessage(transportProtocol, len);
//  if (1 /*check result*/) {
//    CU_ASSERT(0);
//  }
//}
//

//
//void testCreateVariableData() {
//  const uint8_t* data;
//  VariableData* result = createVariableData(data);
//  if (1 /*check result*/) {
//    CU_ASSERT(0);
//  }
//}
//
//void testCreateTaskCreateMessage() {
//  const uint8_t* topic;
//  TaskCreateMessage* result = createTaskCreateMessage(topic);
//  if (1 /*check result*/) {
//    CU_ASSERT(0);
//  }
//}
//
//void testFreeMessage() {
//  Message* message;
//  freeMessage(message);
//  if (1 /*check result*/) {
//    CU_ASSERT(0);
//  }
//}
//
//void testFreeTransportProtocol() {
//  TransportProtocol* transportProtocol;
//  freeTransportProtocol(transportProtocol);
//  if (1 /*check result*/) {
//    CU_ASSERT(0);
//  }
//}
//
//void testFreeVariableData() {
//  VariableData* variableData;
//  freeVariableData(variableData);
//  if (1 /*check result*/) {
//    CU_ASSERT(0);
//  }
//}
//
//void testFreeTaskCreateMessage() {
//  TaskCreateMessage* taskCreateMessage;
//  freeTaskCreateMessage(taskCreateMessage);
//  if (1 /*check result*/) {
//    CU_ASSERT(0);
//  }
//}
//
//void testFreeSingleTaskServerAckMessage() {
//  SingleTaskServerAckMessage* ack;
//  freeSingleTaskServerAckMessage(ack);
//  if (1 /*check result*/) {
//    CU_ASSERT(0);
//  }
//}
//
//void testFreePollAndLockTaskMessage() {
//  PollAndLockTaskMessage* pollAndLockTaskMessage;
//  freePollAndLockTaskMessage(pollAndLockTaskMessage);
//  if (1 /*check result*/) {
//    CU_ASSERT(0);
//  }
//}
//
//void testWriteTaskCreateMessage() {
//  uint8_t* buffer;
//  TaskCreateMessage* taskCreateMessage;
//  uint8_t* result = writeTaskCreateMessage(buffer, taskCreateMessage);
//  if (1 /*check result*/) {
//    CU_ASSERT(0);
//  }
//}
//
//void testWriteMessage() {
//  uint8_t* buffer;
//  Message* message;
//  uint8_t* result = writeMessage(buffer, message);
//  if (1 /*check result*/) {
//    CU_ASSERT(0);
//  }
//}
//
//void testWriteTransportProtocol() {
//  uint8_t* buffer;
//  TransportProtocol* transportProtocol;
//  uint8_t* result = writeTransportProtocol(buffer, transportProtocol);
//  if (1 /*check result*/) {
//    CU_ASSERT(0);
//  }
//}
//
//void testWriteVariableData() {
//  uint8_t* buffer;
//  VariableData* data;
//  uint8_t* result = writeVariableData(buffer, data);
//  if (1 /*check result*/) {
//    CU_ASSERT(0);
//  }
//}
//
//void testWritePollAndLockTaskMessage() {
//  uint8_t* buffer;
//  PollAndLockTaskMessage* pollAndLock;
//  writePollAndLockTaskMessage(buffer, pollAndLock);
//  if (1 /*check result*/) {
//    CU_ASSERT(0);
//  }
//}
//
//void testReadMessage() {
//  uint8_t* buffer;
//  Message* message;
//  uint8_t* result = readMessage(buffer, message);
//  if (1 /*check result*/) {
//    CU_ASSERT(0);
//  }
//}
//
//void testReadTransportProtocol() {
//  uint8_t* buffer;
//  TransportProtocol* transportProtocol;
//  uint8_t* result = readTransportProtocol(buffer, transportProtocol);
//  if (1 /*check result*/) {
//    CU_ASSERT(0);
//  }
//}
//
//void testReadSingleTaskServerAckMessage() {
//  uint8_t* buffer;
//  SingleTaskServerAckMessage* ack;
//  uint8_t* result = readSingleTaskServerAckMessage(buffer, ack);
//  if (1 /*check result*/) {
//    CU_ASSERT(0);
//  }
//}

int main() {
  CU_pSuite pSuite = NULL;

  /* Initialize the CUnit test registry */
  if (CUE_SUCCESS != CU_initialize_registry())
    return CU_get_error();

  /* Add a suite to the registry */
  pSuite = CU_add_suite("protocolTest", init_suite, clean_suite);
  if (NULL == pSuite) {
    CU_cleanup_registry();
    return CU_get_error();
  }

  /* Add the tests to the suite */
  if ((NULL == CU_add_test(pSuite, "testCreateTransportProtocol", testCreateTransportProtocol))
//          ||
//          (NULL == CU_add_test(pSuite, "testCreateMessage", testCreateMessage)) ||
//          (NULL == CU_add_test(pSuite, "testCreateVariableData", testCreateVariableData)) ||
//          (NULL == CU_add_test(pSuite, "testCreateTaskCreateMessage", testCreateTaskCreateMessage)) ||
//          (NULL == CU_add_test(pSuite, "testFreeMessage", testFreeMessage)) ||
//          (NULL == CU_add_test(pSuite, "testFreeTransportProtocol", testFreeTransportProtocol)) ||
//          (NULL == CU_add_test(pSuite, "testFreeVariableData", testFreeVariableData)) ||
//          (NULL == CU_add_test(pSuite, "testFreeTaskCreateMessage", testFreeTaskCreateMessage)) ||
//          (NULL == CU_add_test(pSuite, "testFreeSingleTaskServerAckMessage", testFreeSingleTaskServerAckMessage)) ||
//          (NULL == CU_add_test(pSuite, "testFreePollAndLockTaskMessage", testFreePollAndLockTaskMessage)) ||
//          (NULL == CU_add_test(pSuite, "testWriteTaskCreateMessage", testWriteTaskCreateMessage)) ||
//          (NULL == CU_add_test(pSuite, "testWriteMessage", testWriteMessage)) ||
//          (NULL == CU_add_test(pSuite, "testWriteTransportProtocol", testWriteTransportProtocol)) ||
//          (NULL == CU_add_test(pSuite, "testWriteVariableData", testWriteVariableData)) ||
//          (NULL == CU_add_test(pSuite, "testWritePollAndLockTaskMessage", testWritePollAndLockTaskMessage)) ||
//          (NULL == CU_add_test(pSuite, "testReadMessage", testReadMessage)) ||
//          (NULL == CU_add_test(pSuite, "testReadTransportProtocol", testReadTransportProtocol)) ||
//          (NULL == CU_add_test(pSuite, "testReadSingleTaskServerAckMessage", testReadSingleTaskServerAckMessage))
      ) {
    CU_cleanup_registry();
    return CU_get_error();
  }

  /* Run all tests using the CUnit Basic interface */
  CU_basic_set_mode(CU_BRM_VERBOSE);
  CU_basic_run_tests();
  CU_cleanup_registry();
  return CU_get_error();
}
