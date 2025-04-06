#ifndef ZOMBIE_STAGE_STATE_H
#define ZOMBIE_STAGE_STATE_H

enum ZombieStage {
  STAGE_INIT,
  STAGE_POWER,
  STAGE_SECURITY,
  STAGE_COMMUNICATION,
  STAGE_COMPLETE
};

extern ZombieStage currentZombieStage;

#endif
