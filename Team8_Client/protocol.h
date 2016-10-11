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
 * Defines the tngp protocol and
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

#include <stdlib.h>
#include "serialization.h"

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
  };

  struct VariableData {
    int16_t length;
    char* data;
  };

#define TASK_CREATE_HEADER_LEN TASK_CREATE_HEADER_TYPE_LEN + TASK_CREATE_HEADER_PAYLOAD_LEN
#define TASK_CREATE_HEADER_TYPE_LEN 2
#define TASK_CREATE_HEADER_PAYLOAD_LEN 2

  struct TaskCreateMessage {
    //header
    struct Message* head;
    //task type
    struct VariableData* taskType;
    //payload
    struct VariableData* payload;
  };

#define SERVER_ACK_LEN MSG_HEADER_LEN + TRANS_HEADER_LEN + 8 + 8

  struct SingleTaskServerAckMessage {
    //header
    struct Message* head;
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
  uint8_t* writeMessage(uint8_t* buffer, struct Message* message);

  /**
   * Writes the given transport protocol into the buffer.
   *
   * @param buffer the buffer that should contain the transport protocol
   * @param transportProtocol the transport protocol which should be written
   */
  uint8_t* writeTransportProtocol(uint8_t* buffer, struct TransportProtocol* transportProtocol);

  /**
   * Writes the variable data into the buffer.
   *
   * @param buffer the buffer that should contain the variable data
   * @param data the variable data which should be written
   */
  uint8_t* writeVariableData(uint8_t* buffer, struct VariableData* data);

  uint8_t* writeTaskCreateMessage(uint8_t* buffer, struct TaskCreateMessage* taskCreateMessage);

  /**
   * Writes the poll and lock message into the buffer.
   *
   * @param buffer the buffer that should contain the variable data
   * @param pollAndLock the poll and lock message which should be written
   */
  void writePollAndLockTaskMessage(uint8_t* buffer, struct PollAndLockTaskMessage* pollAndLock);



uint8_t* readMessage(uint8_t* buffer,struct Message* serverAck);
uint8_t* readTransportProtocol(uint8_t* buffer, struct TransportProtocol* transportProtocol);
uint8_t* readSingleTaskServerAckMessage(uint8_t* buffer, struct SingleTaskServerAckMessage* ack);
#ifdef __cplusplus
}
#endif

#endif /* PROTOCOL_H */

