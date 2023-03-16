#pragma once

#include "J2534_v0404.h"

namespace j2534 {

class BaseMessage {
public:
  virtual std::vector<PASSTHRU_MSG> toPassThruMsgs(unsigned long ProtocolID,
                                           unsigned long Flags) const = 0;
};

}
