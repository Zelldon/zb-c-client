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

#include "protocol.h"

// write ///////////////////////////////////////////////////////////////////////

uint8_t* writeTaskCreateMessage(uint8_t* buffer, struct TaskCreateMessage* taskCreateMessage) {
  uint8_t* nextFreeBuff = writeMessage(buffer, taskCreateMessage->head);
  nextFreeBuff = writeVariableData(nextFreeBuff, taskCreateMessage->taskType);
  return writeVariableData(nextFreeBuff, taskCreateMessage->payload);
}

uint8_t* writeMessage(uint8_t* buffer, struct Message* message) {
  //message header
  uint8_t* nextFreeBuff = serialize_int32(buffer, message->len);
  nextFreeBuff = serialize_int8(nextFreeBuff, message->version);
  nextFreeBuff = serialize_int8(nextFreeBuff, message->flags);
  nextFreeBuff = serialize_int16(nextFreeBuff, message->type);
  nextFreeBuff = serialize_int32(nextFreeBuff, message->streamId);

  //body
  return writeTransportProtocol(nextFreeBuff, message->body);
}

uint8_t* writeTransportProtocol(uint8_t* buffer, struct TransportProtocol* transportProtocol) {
  //transport protocol header
  uint8_t* nextFreeBuff = serialize_int16(buffer, transportProtocol->protocolId);
  nextFreeBuff = serialize_int64(nextFreeBuff, transportProtocol->connectionId);
  nextFreeBuff = serialize_int64(nextFreeBuff, transportProtocol->requestId);
  nextFreeBuff = serialize_int16(nextFreeBuff, transportProtocol->blockLength);
  nextFreeBuff = serialize_int16(nextFreeBuff, transportProtocol->templateId);
  nextFreeBuff = serialize_int16(nextFreeBuff, transportProtocol->schemaId);
  nextFreeBuff = serialize_int16(nextFreeBuff, transportProtocol->version);
  nextFreeBuff = serialize_int16(nextFreeBuff, transportProtocol->resourceId);
  return serialize_int16(nextFreeBuff, transportProtocol->shardId);
}

uint8_t* writeVariableData(uint8_t* buffer, struct VariableData* data) {
  uint8_t* nextFreeBuff = serialize_int16(buffer, data->length);
  memcpy(nextFreeBuff, data->data, data->length);
  return nextFreeBuff + data->length;
}

void writePollAndLockTaskMessage(uint8_t* buffer, struct PollAndLockTaskMessage* pollAndLock) {
  uint8_t* nextFreeBuff = serialize_int16(buffer, pollAndLock->consumerId);
  nextFreeBuff = serialize_int64(nextFreeBuff, pollAndLock->lockTime);
  nextFreeBuff = serialize_int16(nextFreeBuff, pollAndLock->maxTasks);
  //task type
  writeVariableData(nextFreeBuff, pollAndLock->taskType);
}



// read ////////////////////////////////////////////////////////////////////////

uint8_t* readMessage(uint8_t* buffer, struct Message* message) {
  //message header
  uint8_t* nextFreeBuff = deserialize_int32(buffer, &message->len);
  nextFreeBuff = deserialize_int8(nextFreeBuff, &message->version);
  nextFreeBuff = deserialize_int8(nextFreeBuff, &message->flags);
  nextFreeBuff = deserialize_int16(nextFreeBuff, &message->type);
  nextFreeBuff = deserialize_int32(nextFreeBuff, &message->streamId);

  //body
  message->body = malloc(sizeof (struct TransportProtocol));
  if (message->body == NULL) {
    return NULL;
  }
  return readTransportProtocol(nextFreeBuff, message->body);
}

uint8_t* readTransportProtocol(uint8_t* buffer, struct TransportProtocol* transportProtocol) {
  //transport protocol header
  uint8_t* nextFreeBuff = deserialize_int16(buffer, &transportProtocol->protocolId);
  nextFreeBuff = deserialize_int64(nextFreeBuff, &transportProtocol->connectionId);
  nextFreeBuff = deserialize_int64(nextFreeBuff, &transportProtocol->requestId);
  nextFreeBuff = deserialize_int16(nextFreeBuff, &transportProtocol->blockLength);
  nextFreeBuff = deserialize_int16(nextFreeBuff, &transportProtocol->templateId);
  nextFreeBuff = deserialize_int16(nextFreeBuff, &transportProtocol->schemaId);
  nextFreeBuff = deserialize_int16(nextFreeBuff, &transportProtocol->version);
  nextFreeBuff = deserialize_int16(nextFreeBuff, &transportProtocol->resourceId);
  return deserialize_int16(nextFreeBuff, &transportProtocol->shardId);
}

uint8_t* readSingleTaskServerAckMessage(uint8_t* buffer, struct SingleTaskServerAckMessage* ack) {
  ack->head = malloc(sizeof (struct Message));
  if (ack->head == NULL) {
    return NULL;
  }
  uint8_t* nextFreeBuff = readMessage(buffer, ack->head);
  return deserialize_int64(nextFreeBuff, &ack->taskId);
}
