/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/*
 * Defines the tngp protocol and some util classes to serialize and
 * deserialize values to and from bytes. Admit that int32_t is 4 bytes.
 * Uses little endian for serialization.
 *
 * File:   protocol.h
 * Author: Christopher Zell <christopher.zell@camunda.com>
 *
 * Created on October 10, 2016, 10:42 AM
 */

#ifndef PROTOCOL_H
#define PROTOCOL_H

#ifdef __cplusplus
extern "C" {
#endif


//#include <sys/types.h>
#include <stdint.h>
#include <string.h>


#define uint8_t unsigned char
#define CREATE_TASK_REQUEST 0
#define POLL_AND_LOCK_REQUEST 1
#define MSG_HEADER_LEN 12

  struct Message {
    //head
    int32_t len;
    int8_t version;
    int8_t flags;
    int16_t type;
    int32_t streamId;
    //body
    struct TransportProtocol* body;
  };

#define TRANS_HEADER_LEN 30

  struct TransportProtocol {
    int16_t protocolId;
    int64_t connectionId;
    int64_t requestId;

    //header
    int16_t blockLength;
    int16_t templateId;
    int16_t schemaId;
    int16_t version;
    int16_t resourceId;
    int16_t shardId;
    //body
    int64_t bodyLen;
    char* body;

    
  };

  struct VariableData {
    int16_t length;
    char* data;
  };

#define TASK_CREATE_HEADER_LEN TASK_CREATE_HEADER_TYPE_LEN + TASK_CREATE_HEADER_PAYLOAD_LEN
#define TASK_CREATE_HEADER_TYPE_LEN 2
#define TASK_CREATE_HEADER_PAYLOAD_LEN 2

  struct TaskCreateMessage {
    //task type
    struct VariableData* taskType;
    //payload
    struct VariableData* payload;
  };

#define SERVER_ACK_LEN 8

  struct SingleTaskServerAckMessage {
    int64_t taskId;
  };


#define POLL_AND_LOCK_HEADER_LEN 12
#define POLL_AND_LOCK_TYPE_LEN 2

  struct PollAndLockTaskMessage {
    int16_t consumerId;
    int64_t lockTime;
    int16_t maxTasks;

    //task type
    struct VariableData* taskType;
  };


#define LOCKED_TASK_BATCH_HEADER 21

  struct LockedTaskBatchMessage {
    int16_t consumerId;
    int64_t lockTime;
    int16_t blockLength;
    int8_t numInGroup;
    //prototype
    int64_t taskId;
  };


  /**
   * Writes the given message into the buffer.
   *
   * @param buffer the buffer that should contain the message
   * @param message the message which should be written
   */
  void writeMessage(uint8_t* buffer, struct Message* message);

  /**
   * Writes the given transport protocol into the buffer.
   *
   * @param buffer the buffer that should contain the transport protocol
   * @param transportProtocol the transport protocol which should be written
   */
  void writeTransportProtocol(uint8_t* buffer, struct TransportProtocol* transportProtocol);

  /**
   * Writes the variable data into the buffer.
   *
   * @param buffer the buffer that should contain the variable data
   * @param data the variable data which should be written
   */
  void writeVariableData(uint8_t* buffer, struct VariableData* data);

  /**
   * Writes the poll and lock message into the buffer.
   *
   * @param buffer the buffer that should contain the variable data
   * @param pollAndLock the poll and lock message which should be written
   */
  void writePollAndLockTaskMessage(uint8_t* buffer, struct PollAndLockTaskMessage* pollAndLock);

  /**
   * Serializes a int64_t value, as little endian, into the given buffer.
   * Long is expected as 8 byte value.
   *
   * @param buffer the buffer which should contain the value
   * @param value the value which should be written
   * @return the next free space in the buffer
   */
  uint8_t* serialize_int64(uint8_t * buffer, int64_t value);

  /**
   * Serializes a int32_t value, as little endian, into the given buffer.
   * Integer is expected as 4 byte value.
   *
   * @param buffer the buffer which should contain the value
   * @param value the value which should be written
   * @return the next free space in the buffer
   */
  uint8_t* serialize_int32(uint8_t * buffer, int32_t value);

  /**
   * Serializes a int16_t value, as little endian, into the given buffer.
   * Short is expected as 2 byte value.
   *
   * @param buffer the buffer which should contain the value
   * @param value the value which should be written
   * @return the next free space in the buffer
   */
  uint8_t* serialize_int16(uint8_t * buffer, int16_t value);

  /**
   * Serializes a int8_t value into the given buffer.
   * Char is expected as 1 byte value.
   *
   * @param buffer the buffer which should contain the value
   * @param value the value which should be written
   * @return the next free space in the buffer
   */
  uint8_t* serialize_int8(uint8_t * buffer, int8_t value);



void readMessage(uint8_t* buffer,struct Message* serverAck);
void readTransportProtocol(uint8_t* buffer, struct TransportProtocol* transportProtocol);
void readSingleTaskServerAckMessage(uint8_t* buffer, struct SingleTaskServerAckMessage* ack);
uint8_t* deserialize_int64(uint8_t * buffer, uint64_t* value);
uint8_t* deserialize_int32(uint8_t * buffer, int32_t* value);
uint8_t* deserialize_int16(uint8_t * buffer, int16_t* value);
uint8_t* deserialize_int8(uint8_t * buffer, uint8_t* value);

#ifdef __cplusplus
}
#endif

#endif /* PROTOCOL_H */

