/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */


#include <sys/types.h>

#include "protocol.h"

// write ///////////////////////////////////////////////////////////////////////

void writeMessage(uint8_t* buffer, struct Message* message) {

  //message header
  uint8_t* nextFreeBuff = serialize_int32(buffer, message->len);
  nextFreeBuff = serialize_int8(nextFreeBuff, message->version);
  nextFreeBuff = serialize_int8(nextFreeBuff, message->flags);
  nextFreeBuff = serialize_int16(nextFreeBuff, message->type);
  nextFreeBuff = serialize_int32(nextFreeBuff, message->streamId);

  //body
  writeTransportProtocol(nextFreeBuff, message->body);
}

void writeTransportProtocol(uint8_t* buffer, struct TransportProtocol* transportProtocol) {
  //transport protocol header
  uint8_t* nextFreeBuff = serialize_int16(buffer, transportProtocol->protocolId);
  nextFreeBuff = serialize_int64(nextFreeBuff, transportProtocol->connectionId);
  nextFreeBuff = serialize_int64(nextFreeBuff, transportProtocol->requestId);
  nextFreeBuff = serialize_int16(nextFreeBuff, transportProtocol->blockLength);
  nextFreeBuff = serialize_int16(nextFreeBuff, transportProtocol->templateId);
  nextFreeBuff = serialize_int16(nextFreeBuff, transportProtocol->schemaId);
  nextFreeBuff = serialize_int16(nextFreeBuff, transportProtocol->version);
  nextFreeBuff = serialize_int16(nextFreeBuff, transportProtocol->resourceId);
  nextFreeBuff = serialize_int16(nextFreeBuff, transportProtocol->shardId);
}

void writeVariableData(uint8_t* buffer, struct VariableData* data) {
  uint8_t* nextFreeBuff = serialize_int16(buffer, data->length);
  memcpy(nextFreeBuff, data->data, data->length);
}

void writePollAndLockTaskMessage(uint8_t* buffer, struct PollAndLockTaskMessage* pollAndLock) {
  uint8_t* nextFreeBuff = serialize_int16(buffer, pollAndLock->consumerId);
  nextFreeBuff = serialize_int64(nextFreeBuff, pollAndLock->lockTime);
  nextFreeBuff = serialize_int16(nextFreeBuff, pollAndLock->maxTasks);
  //task type
  writeVariableData(nextFreeBuff, pollAndLock->taskType);
}



// read ////////////////////////////////////////////////////////////////////////

void readMessage(uint8_t* buffer, struct Message* message) {

  //message header
  uint8_t* nextFreeBuff = deserialize_int32(buffer, &message->len);
  nextFreeBuff = deserialize_int8(nextFreeBuff, &message->version);
  nextFreeBuff = deserialize_int8(nextFreeBuff, &message->flags);
  nextFreeBuff = deserialize_int16(nextFreeBuff, &message->type);
  nextFreeBuff = deserialize_int32(nextFreeBuff, &message->streamId);

  //body
  readTransportProtocol(nextFreeBuff, message->body);
}

void readTransportProtocol(uint8_t* buffer, struct TransportProtocol* transportProtocol) {
  //transport protocol header
  uint8_t* nextFreeBuff = deserialize_int16(buffer, &transportProtocol->protocolId);
  nextFreeBuff = deserialize_int64(nextFreeBuff, &transportProtocol->connectionId);
  nextFreeBuff = deserialize_int64(nextFreeBuff, &transportProtocol->requestId);
  nextFreeBuff = deserialize_int16(nextFreeBuff, &transportProtocol->blockLength);
  nextFreeBuff = deserialize_int16(nextFreeBuff, &transportProtocol->templateId);
  nextFreeBuff = deserialize_int16(nextFreeBuff, &transportProtocol->schemaId);
  nextFreeBuff = deserialize_int16(nextFreeBuff, &transportProtocol->version);
  nextFreeBuff = deserialize_int16(nextFreeBuff, &transportProtocol->resourceId);
  nextFreeBuff = deserialize_int16(nextFreeBuff, &transportProtocol->shardId);
}

void readSingleTaskServerAckMessage(uint8_t* buffer, struct SingleTaskServerAckMessage* ack) {
  deserialize_int64(buffer, &ack->taskId);
}

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

uint8_t* deserialize_int64(uint8_t * buffer, u_int64_t* value) {
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