#ifndef EH_SIM_CHECKPOINT_SCHEME_HPP
#define EH_SIM_CHECKPOINT_SCHEME_HPP

#include <thumbulator/cpu.hpp>

class checkpoint_scheme {
public:
  double backup_energy() const
  {
    return 0.0;
  }

  void backup()
  {
    architectural_state = thumbulator::cpu;
  }

  double restore_energy() const
  {
    return 0.0;
  }

  void restore()
  {
    thumbulator::cpu = architectural_state;
  }
private:
  thumbulator::cpu_state architectural_state;
};

#endif //EH_SIM_CHECKPOINT_SCHEME_HPP
