#pragma once

#include "bt.hpp"
#include "flashlog.hpp"

class TransferController {
  public:
    void begin();
    void run();
    bool isDone();

  private:
    bool _done = true;
    uint32_t _currAddr = LOG_O;
    int _numofEntries = 0;
};