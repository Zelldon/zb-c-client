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

#define CREATE_TASK_REQUEST 0
#define POLL_AND_LOCK_REQUEST 1

#define MSG_HEADER_LEN 12

  /**
   * Defines the main component with all necessary headers,
   * to communicate with the broker.
   */
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

  /**
   * Defines the transport protocol with all necessary headers.
   */
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

  /**
   * Defines the structure of variable data.
   */
  struct VariableData {
    int16_t length;
    char* data;
  };

#define TASK_CREATE_HEADER_LEN TASK_CREATE_HEADER_TYPE_LEN + TASK_CREATE_HEADER_PAYLOAD_LEN
#define TASK_CREATE_HEADER_TYPE_LEN 2
#define TASK_CREATE_HEADER_PAYLOAD_LEN 2

  /**
   * Defines the message to create a task.
   */
  struct TaskCreateMessage {
    //header
    struct Message* head;
    //task type
    struct VariableData* taskType;
    //payload
    struct VariableData* payload;
  };

#define SERVER_ACK_LEN MSG_HEADER_LEN + TRANS_HEADER_LEN + 8 + 8

  /**
   * Defines the acknowledgment of the broker which is send after
   * a task is created.
   */
  struct SingleTaskServerAckMessage {
    //header
    struct Message* head;
    int64_t taskId;
  };


#define POLL_AND_LOCK_HEADER_LEN 12
#define POLL_AND_LOCK_TYPE_LEN 2

  /**
   * Defines the message to poll and lock an existing task.
   */
  struct PollAndLockTaskMessage {
    int16_t consumerId;
    int64_t lockTime;
    int16_t maxTasks;

    //task type
    struct VariableData* taskType;
  };


#define LOCKED_TASK_BATCH_HEADER 21

  /**
   * Defines the response of the broker which is send after
   * a task is polled and locked.
   */
  struct LockedTaskBatchMessage {
    int16_t consumerId;
    int64_t lockTime;
    int16_t blockLength;
    int8_t numInGroup;
    //prototype
    int64_t taskId;
  };

  // create structures /////////////////////////////////////////////////////////

  /**
   * Creates a message structure, with the given transport protocol.
   *
   * @param transportProtocol the transport protocol, which should be a composite of the message structure
   * @param len the length of the message
   * @return a pointer of the created message structure
   */
  struct Message* createMessage(struct TransportProtocol* transportProtocol, int32_t len);

  /**
   * Creates a transport protocol structure, with the given template id.
   *
   * @param templateId the template id for the transport protocol
   * @return a pointer of the create transport protocol structure
   */
  struct TransportProtocol* createTransportProtocol(int16_t templateId);

  /**
   * Creates a variable data structure, with the given data.
   *
   * @param data the data for the variable data structure
   * @return a pointer of the create variable structure
   */
  struct VariableData* createVariableData(const uint8_t* data);

  /**
   * Creates a task create message structure, with the given topic.
   *
   * @param topic the topic for the task create message structure
   * @return a pointer of the create variable structure
   */
  struct TaskCreateMessage* createTaskCreateMessage(const uint8_t* topic);

  // free structures ///////////////////////////////////////////////////////////

  /**
   * Frees the allocated memory for the given message structure.
   *
   * @param message the pointer to the structure which should be freed
   */
  void freeMessage(struct Message* message);

  /**
   * Frees the allocated memory for the given transport protocol structure.
   *
   * @param transportProtocol the pointer to the structure which should be freed
   */
  void freeTransportProtocol(struct TransportProtocol* transportProtocol);

  /**
   * Frees the allocated memory for the given variable data structure.
   *
   * @param variableData the pointer to the structure which should be freed
   */
  void freeVariableData(struct VariableData* variableData);

  /**
   * Frees the allocated memory for the given task create message structure.
   *
   * @param taskCreateMessage the pointer to the structure which should be freed
   */
  void freeTaskCreateMessage(struct TaskCreateMessage* taskCreateMessage);

  /**
   * Frees the allocated memory for the given single task server ack message structure.
   *
   * @param ack the pointer to the structure which should be freed
   */
  void freeSingleTaskServerAckMessage(struct SingleTaskServerAckMessage* ack);

  /**
   * Frees the allocated memory for the given poll and lock task message structure.
   *
   * @param pollAndLockTaskMessage the pointer to the structure which should be freed
   */
  void freePollAndLockTaskMessage(struct PollAndLockTaskMessage* pollAndLockTaskMessage);

  // write structures //////////////////////////////////////////////////////////

  /**
   * Writes the given message into the buffer.
   *
   * @param buffer the buffer that should contain the message
   * @param message the message which should be written
   * @return the pointer which points on the next space in the buffer
   */
  uint8_t* writeMessage(uint8_t* buffer, struct Message* message);

  /**
   * Writes the given transport protocol into the buffer.
   *
   * @param buffer the buffer that should contain the transport protocol
   * @param transportProtocol the transport protocol which should be written
   * @return the pointer which points on the next space in the buffer
   */
  uint8_t* writeTransportProtocol(uint8_t* buffer, struct TransportProtocol* transportProtocol);

  /**
   * Writes the variable data into the buffer.
   *
   * @param buffer the buffer that should contain the variable data
   * @param data the variable data which should be written
   * @return the pointer which points on the next space in the buffer
   */
  uint8_t* writeVariableData(uint8_t* buffer, struct VariableData* data);

  /**
   * Writes the task creation message into the buffer.
   *
   * @param buffer the buffer that should contain the task creation message
   * @param taskCreateMessage the task creation message which should be written
   * @return the pointer which points on the next space in the buffer
   */
  uint8_t* writeTaskCreateMessage(uint8_t* buffer, struct TaskCreateMessage* taskCreateMessage);

  /**
   * Writes the poll and lock message into the buffer.
   *
   * @param buffer the buffer that should contain the variable data
   * @param pollAndLock the poll and lock message which should be written
   */
  void writePollAndLockTaskMessage(uint8_t* buffer, struct PollAndLockTaskMessage* pollAndLock);


  /**
   * Reads the message from the buffer and
   * writes into the structure on which the pointer points to.
   *
   * @param buffer the buffer that contain the message
   * @param message the pointer which points to the message structure
   * @return the pointer which points to the next place in the buffer
   */
  uint8_t* readMessage(uint8_t* buffer, struct Message* message);

  /**
   *
   * Reads the transport protocol from the buffer and
   * writes into the structure on which the pointer points to.
   *
   * @param buffer the buffer that contain the transport protocol
   * @param transportProtocol the pointer which points to the transport protocol structure
   * @return the pointer which points to the next place in the buffer
   */
  uint8_t* readTransportProtocol(uint8_t* buffer, struct TransportProtocol* transportProtocol);

  /**
   *
   * Reads the acknowledge message from the buffer and
   * writes into the structure on which the pointer points to.
   *
   * @param buffer the buffer that contain the transport protocol
   * @param ack the pointer which points to the acknowledge message structure
   * @return the pointer which points to the next place in the buffer
   */
  uint8_t* readSingleTaskServerAckMessage(uint8_t* buffer, struct SingleTaskServerAckMessage* ack);

#ifdef __cplusplus
}
#endif

#endif /* PROTOCOL_H */

