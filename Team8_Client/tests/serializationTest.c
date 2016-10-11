/*
 * File:   serializationTest.c
 * Author: Christopher Zell <christopher.zell@camunda.com>
 *
 * Created on Oct 11, 2016, 12:29:28 PM
 */

#include <stdio.h>
#include <stdlib.h>
#include <CUnit/Basic.h>
#include "../serialization.h"
#include <math.h>
/*
 * CUnit Test Suite
 */


int init_suite(void) {
  return 0;
}

int clean_suite(void) {
  return 0;
}

// serialize ///////////////////////////////////////////////////////////////////

void testSerialize_int64() {
  uint8_t buffer[8] = {0, 0, 0, 0, 0, 0, 0, 0};
  int64_t value = (int64_t) powf(256, 7);
  serialize_int64(buffer, value);
  CU_ASSERT_EQUAL(1, buffer[7]);
}

void testSerialize_int32() {
  uint8_t buffer[4] = {0, 0, 0, 0};
  int32_t value = (int64_t) powf(256, 3);
  serialize_int32(buffer, value);
  CU_ASSERT_EQUAL(1, buffer[3]);
}

void testSerialize_int16() {
  uint8_t buffer[2] = {0, 0};
  int16_t value = 256;
  serialize_int16(buffer, value);
  CU_ASSERT_EQUAL(1, buffer[1]);
}

void testSerialize_int8() {
  uint8_t buffer[1] = {0};
  int8_t value = 255;
  serialize_int8(buffer, value);
  CU_ASSERT_EQUAL(255, buffer[0]);
}

// deserialize /////////////////////////////////////////////////////////////////

void testDeserialize_int64() {
  uint8_t buffer[8] = {0, 0, 0, 0, 0, 0, 0, 1};
  int64_t value;
  deserialize_int64(buffer, &value);
  CU_ASSERT_EQUAL((int64_t) powf(256, 7), value);
}


void testDeserialize_int32() {
  uint8_t buffer[4] = {0, 0, 0, 1};
  int32_t value;
  deserialize_int32(buffer, &value);
  CU_ASSERT_EQUAL((int32_t) powf(256, 3), value);
}

void testDeserialize_int16() {
  uint8_t buffer[2] = {0, 1};
  int16_t value;
  deserialize_int16(buffer, &value);
  CU_ASSERT_EQUAL(256, value);
}

void testDeserialize_int8() {
  uint8_t buffer[1] = {255};
  uint8_t value;
  deserialize_int8(buffer, &value);
  CU_ASSERT_EQUAL(255, value);
}

int main() {
  CU_pSuite pSuite = NULL;

  /* Initialize the CUnit test registry */
  if (CUE_SUCCESS != CU_initialize_registry())
    return CU_get_error();

  /* Add a suite to the registry */
  pSuite = CU_add_suite("serializationTest", init_suite, clean_suite);
  if (NULL == pSuite) {
    CU_cleanup_registry();
    return CU_get_error();
  }

  /* Add the tests to the suite */
  if ((NULL == CU_add_test(pSuite, "testSerialize_int64", testSerialize_int64)) ||
          (NULL == CU_add_test(pSuite, "testSerialize_int32", testSerialize_int32)) ||
          (NULL == CU_add_test(pSuite, "testSerialize_int16", testSerialize_int16)) ||
          (NULL == CU_add_test(pSuite, "testSerialize_int8", testSerialize_int8)) ||
          (NULL == CU_add_test(pSuite, "testDeserialize_int64", testDeserialize_int64)) ||
          (NULL == CU_add_test(pSuite, "testDeserialize_int32", testDeserialize_int32)) ||
          (NULL == CU_add_test(pSuite, "testDeserialize_int16", testDeserialize_int16)) ||
          (NULL == CU_add_test(pSuite, "testDeserialize_int8", testDeserialize_int8))
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
