/*
 * Copyright 2016 Christopher Zell <christopher.zell@camunda.com>.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */
#include "serialization.h"

// serialize ///////////////////////////////////////////////////////////////////

uint8_t* serialize_int64(uint8_t * buffer, int64_t value) {
  buffer[7] = value >> 56;
  buffer[6] = value >> 48;
  buffer[5] = value >> 40;
  buffer[4] = value >> 32;
  buffer[3] = value >> 24;
  buffer[2] = value >> 16;
  buffer[1] = value >> 8;
  buffer[0] = value;
  return buffer + 8;
}

uint8_t* serialize_int32(uint8_t * buffer, int32_t value) {
  buffer[3] = value >> 24;
  buffer[2] = value >> 16;
  buffer[1] = value >> 8;
  buffer[0] = value;
  return buffer + 4;
}

uint8_t* serialize_int16(uint8_t * buffer, int16_t value) {
  buffer[1] = value >> 8;
  buffer[0] = value;
  return buffer + 2;
}

uint8_t* serialize_int8(uint8_t * buffer, int8_t value) {
  buffer[0] = value;
  return buffer + 1;
}


// deserialize /////////////////////////////////////////////////////////////////

uint8_t* deserialize_int64(uint8_t * buffer, uint64_t* value) {
  *value = *((uint64_t*) buffer);
  return buffer + 8;
}

uint8_t* deserialize_int32(uint8_t * buffer, int32_t* value) {
  char b[4];
  memcpy(b, buffer, 4);
  *value = 0;
  *value |= buffer[3] >> 24;
  *value |= buffer[2] >> 16;
  *value |= buffer[1] >> 8;
  *value |= buffer[0];
  return buffer + 4;
}

uint8_t* deserialize_int16(uint8_t * buffer, int16_t* value) {
  *value = 0;
  *value |= buffer[1] >> 8;
  *value |= buffer[0];
  return buffer + 2;
}

uint8_t* deserialize_int8(uint8_t * buffer, uint8_t* value) {
  *value = buffer[0];
  return buffer + 1;
}