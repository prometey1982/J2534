#pragma once

#include "J2534_v0404.h"

namespace j2534 {

class BaseMessage {
public:
  explicit BaseMessage(uint32_t canId)
    : _canId{ canId }
  {
  }

  virtual std::vector<PASSTHRU_MSG>
  toPassThruMsgs(unsigned long ProtocolID, unsigned long Flags) const = 0;

  uint32_t getCanId() const
  {
    return _canId;
  }

private:
  uint32_t _canId;
};

} // namespace j2534
