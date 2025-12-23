#pragma once

#include "BLECharacteristic.h"
#include "BLEService.h"
#include "bluefruit_common.h"

class BLETis : public BLEService {
  protected:
    BLECharacteristic time;
    BLECharacteristic tz;

  public:
};