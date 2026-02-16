#pragma once

class FSM {
  public:
    FSM() : currentState(SystemState::STATE_INIT) {};
    void run();

  private:
    enum SystemState {
        STATE_OFF,
        STATE_INIT,
        STATE_LOW_BATT,
        STATE_CHARGING,
        STATE_IDLE,
        STATE_CONDITION_CHECK,
        STATE_MEASURE_BP,
        STATE_BP_HIGH,
        STATE_BP_CRITICAL,
        STATE_DONE
    };

    SystemState currentState = SystemState::STATE_OFF;
};