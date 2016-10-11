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

/*
 * Defines some util functions to serialize and deserialize values to
 * and from bytes. Uses little endian for serialization and deserialization.
 *
 * File:   serialization.h
 * Author: Christopher Zell <christopher.zell@camunda.com>
 *
 * Created on October 11, 2016, 9:10 AM
 */

#ifndef SERIALIZATION_H
#define SERIALIZATION_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include <string.h>

  /**
   * Serializes a int64_t value, as little endian, into the given buffer.
   *
   * @param buffer the buffer which should contain the value
   * @param value the value which should be written
   * @return the next free space in the buffer
   */
  uint8_t* serialize_int64(uint8_t * buffer, int64_t value);

  /**
   * Serializes a int32_t value, as little endian, into the given buffer.
   *
   * @param buffer the buffer which should contain the value
   * @param value the value which should be written
   * @return the next free space in the buffer
   */
  uint8_t* serialize_int32(uint8_t * buffer, int32_t value);

  /**
   * Serializes a int16_t value, as little endian, into the given buffer.
   *
   * @param buffer the buffer which should contain the value
   * @param value the value which should be written
   * @return the next free space in the buffer
   */
  uint8_t* serialize_int16(uint8_t * buffer, int16_t value);

  /**
   * Serializes a int8_t value into the given buffer.
   *
   * @param buffer the buffer which should contain the value
   * @param value the value which should be written
   * @return the next free space in the buffer
   */
  uint8_t* serialize_int8(uint8_t * buffer, int8_t value);


  /**
   * Deserializes an int64_t value from the given buffer.
   *
   * @param buffer the buffer which contains the value
   * @param value the pointer to the variable on which the value should be written
   * @return the next place in the buffer
   */
  uint8_t* deserialize_int64(uint8_t * buffer, int64_t* value);

  /**
   * Deserializes an int32_t value from the given buffer.
   *
   * @param buffer the buffer which contains the value
   * @param value the pointer to the variable on which the value should be written
   * @return the next place in the buffer
   */
  uint8_t* deserialize_int32(uint8_t * buffer, int32_t* value);

  /**
   * Deserializes an int16_t value from the given buffer.
   *
   * @param buffer the buffer which contains the value
   * @param value the pointer to the variable on which the value should be written
   * @return the next place in the buffer
   */
  uint8_t* deserialize_int16(uint8_t * buffer, int16_t* value);

  /**
   * Deserializes an int8_t value from the given buffer.
   *
   * @param buffer the buffer which contains the value
   * @param value the pointer to the variable on which the value should be written
   * @return the next place in the buffer
   */
  uint8_t* deserialize_int8(uint8_t * buffer, uint8_t* value);


#ifdef __cplusplus
}
#endif

#endif /* SERIALIZATION_H */

