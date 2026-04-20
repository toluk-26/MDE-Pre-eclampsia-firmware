#pragma once

#include "bt.hpp"

class StreamController {
  public:
    void begin();
    void run(uint64_t value);
    bool isDone();
};