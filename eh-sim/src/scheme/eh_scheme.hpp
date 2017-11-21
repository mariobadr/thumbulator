#ifndef EH_SIM_SCHEME_HPP
#define EH_SIM_SCHEME_HPP

namespace ehsim {

class capacitor;
struct stats_bundle;
struct active_stats;

/**
 * An abstract checkpointing scheme.
 */
class eh_scheme {
public:
  virtual capacitor &get_battery() = 0;

  virtual uint32_t clock_frequency() const = 0;

  virtual void execute_instruction(stats_bundle *stats) = 0;

  virtual bool is_active(stats_bundle *stats) = 0;

  virtual bool will_backup(stats_bundle *stats) const = 0;

  virtual uint64_t backup(stats_bundle *stats) = 0;

  virtual uint64_t restore(stats_bundle *stats) = 0;

  virtual double estimate_progress(active_stats const &active_period) const = 0;
};
}

#endif //EH_SIM_SCHEME_HPP
