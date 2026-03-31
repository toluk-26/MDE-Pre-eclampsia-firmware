#pragma once

#include "bt.hpp"

class StreamController {
  public:
    void begin();
    void run();
    bool isDone();
};