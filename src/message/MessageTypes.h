

#ifndef MESSAGETYPES_H_
#define MESSAGETYPES_H_

#include "../config/Config.h"

//#include "../../cmake-build-debug/src/message/HelloMessage.pb.h"
#include "../../cmake-build-debug/src/message/RDMAConnRequest.pb.h"
#include "../../cmake-build-debug/src/message/RDMAConnResponse.pb.h"
#include "../../cmake-build-debug/src/message/RDMAConnRequestMgmt.pb.h"
#include "../../cmake-build-debug/src/message/RDMAConnResponseMgmt.pb.h"
#include "../../cmake-build-debug/src/message/MemoryResourceRequest.pb.h"
#include "../../cmake-build-debug/src/message/MemoryResourceResponse.pb.h"

#include "../../cmake-build-debug/src/message/ErrorMessage.pb.h"
#include "../../cmake-build-debug/src/message/MemoryResourceRequest.pb.h"

#include <google/protobuf/any.pb.h>
#include <google/protobuf/message.h>
#include <iostream>
using google::protobuf::Any;

namespace rdma
{

enum MessageTypesEnum : int
{
  MEMORY_RESOURCE_REQUEST,
  MEMORY_RESOURCE_RELEASE,
};

class MessageTypes
{
public:

  static Any createMemoryResourceRequest(size_t size)
  {
    MemoryResourceRequest resReq;
    resReq.set_size(size);
    resReq.set_type(MessageTypesEnum::MEMORY_RESOURCE_REQUEST);
    Any anyMessage;
    anyMessage.PackFrom(resReq);
    return anyMessage;
  }

  static Any createMemoryResourceRequest(size_t size, std::string &name,
                                         bool persistent)
  {
    MemoryResourceRequest resReq;
    resReq.set_size(size);
    resReq.set_type(MessageTypesEnum::MEMORY_RESOURCE_REQUEST);
    resReq.set_name(name);
    resReq.set_persistent(persistent);

    Any anyMessage;
    anyMessage.PackFrom(resReq);
    return anyMessage;
  }

  static Any createMemoryResourceRelease(size_t size, size_t offset)
  {
    MemoryResourceRequest resReq;
    resReq.set_size(size);
    resReq.set_offset(offset);
    resReq.set_type(MessageTypesEnum::MEMORY_RESOURCE_RELEASE);
    Any anyMessage;
    anyMessage.PackFrom(resReq);
    return anyMessage;
  }
};
}

#endif // MESSAGETYPES_H_
