#include <argagg/argagg.hpp>

#include <fstream>
#include <iomanip>
#include <iostream>
#include <memory>

#include "scheme/backup_every_cycle.hpp"
#include "scheme/clank.hpp"
#include "scheme/parametric.hpp"

#include "simulate.hpp"
#include "voltage_trace.hpp"

void print_usage(std::ostream &stream, argagg::parser const &arguments)
{
  argagg::fmt_ostream help(stream);

  help << "Simulate an energy harvesting environment.\n\n";
  help << "simulate [options] ARG [ARG...]\n\n";
  help << arguments;
}

void ensure_file_exists(std::string const &path_to_file)
{
  std::ifstream binary_file(path_to_file);
  if(!binary_file.good()) {
    throw std::runtime_error("File does not exist: " + path_to_file);
  }
}

void validate(argagg::parser_results const &options)
{
  if(options["binary"].count() == 0) {
    throw std::runtime_error("Missing path to application binary.");
  }

  auto const path_to_binary = options["binary"].as<std::string>();
  ensure_file_exists(path_to_binary);

  if(options["voltages"].count() == 0) {
    throw std::runtime_error("Missing path to voltage trace.");
  }

  auto const path_to_voltage_trace = options["voltages"].as<std::string>();
  ensure_file_exists(path_to_voltage_trace);

  if(options["rate"].count() == 0) {
    throw std::runtime_error("No sampling rate provided for the voltage trace.");
  }
}

int main(int argc, char *argv[])
{
  argagg::parser arguments{{{"help", {"-h", "--help"}, "display help information", 0},
      {"voltages", {"--voltage-trace"}, "path to voltage trace", 1},
      {"rate", {"--voltage-rate"}, "sampling rate of voltage trace (microseconds)", 1},
      {"harvest", {"--always-harvest"}, "harvest during active periods", 1},
      {"scheme", {"--scheme"}, "the checkpointing scheme to use", 1},
      {"tau_B", {"--tau-b"}, "the backup period for the parametric scheme", 1},
      {"binary", {"-b", "--binary"}, "path to application binary", 1},
      {"output", {"-o", "--output"}, "output file", 1}}};

  try {
    auto const options = arguments.parse(argc, argv);
    if(options["help"]) {
      print_usage(std::cout, arguments);
      return EXIT_SUCCESS;
    }

    validate(options);

    auto const path_to_binary = options["binary"];
    bool always_harvest = options["harvest"].as<int>(1) == 1;

    auto const path_to_voltage_trace = options["voltages"];
    std::chrono::milliseconds sampling_period(options["rate"]);

    std::unique_ptr<ehsim::eh_scheme> scheme = nullptr;
    auto const scheme_select = options["scheme"].as<std::string>("bec");
    if(scheme_select == "bec") {
      scheme = std::make_unique<ehsim::backup_every_cycle>();
    } else if(scheme_select == "odab") {
      throw std::runtime_error("ODAB is no longer supported.");
    } else if(scheme_select == "magic") {
      throw std::runtime_error("Magic is no longer supported.");
    } else if(scheme_select == "clank") {
      scheme = std::make_unique<ehsim::clank>();
    } else if(scheme_select == "parametric") {
      auto const tau_b = options["tau_B"].as<int>(1000);
      scheme = std::make_unique<ehsim::parametric>(tau_b);
    } else {
      throw std::runtime_error("Unknown scheme selected.");
    }

    ehsim::voltage_trace power(path_to_voltage_trace, sampling_period);

    auto const stats = ehsim::simulate(path_to_binary, power, scheme.get(), always_harvest);

    std::cout << "CPU instructions executed: " << stats.cpu.instruction_count << "\n";
    std::cout << "CPU time (cycles): " << stats.cpu.cycle_count << "\n";
    std::cout << "Total time (ns): " << stats.system.time.count() << "\n";
    std::cout << "Energy harvested (J): " << stats.system.energy_harvested * 1e-9 << "\n";
    std::cout << "Energy remaining (J): " << stats.system.energy_remaining * 1e-9 << "\n";

    std::string output_file_name(scheme_select + ".csv");
    if(options["output"].count() > 0) {
      output_file_name = options["output"].as<std::string>();
    }

    std::ofstream out(output_file_name);
    out.setf(std::ios::fixed);
    out << "id, E, epsilon, epsilon_C, tau_B, alpha_B, energy_consumed, n_B, tau_P, tau_D, e_P, e_B, "
           "e_R, sim_p, eh_p\n";

    int id = 0;
    for(auto const &model : stats.models) {
      out << id++ << ", ";

      auto const eh_parameters = ehsim::eh_model_parameters(model);
      out << std::setprecision(3) << eh_parameters.E << ", ";
      out << std::setprecision(3) << eh_parameters.epsilon << ", ";
      out << std::setprecision(3) << eh_parameters.epsilon_C << ", ";
      out << std::setprecision(2) << eh_parameters.tau_B << ", ";
      out << std::setprecision(4) << eh_parameters.alpha_B << ", ";

      auto const tau_D = model.time_for_instructions - model.time_forward_progress;
      out << std::setprecision(3) << model.energy_consumed << ", ";
      out << std::setprecision(0) << model.num_backups << ", ";
      out << std::setprecision(0) << model.time_forward_progress << ", ";
      out << std::setprecision(0) << tau_D << ", ";
      out << std::setprecision(3) << model.energy_forward_progress << ", ";
      out << std::setprecision(3) << model.energy_for_backups << ", ";
      out << std::setprecision(3) << model.energy_for_restore << ", ";

      out << std::setprecision(3) << model.progress << ", ";
      out << std::setprecision(3) << model.eh_progress << "\n";
    }
  } catch(std::exception const &e) {
    std::cerr << "Error: " << e.what() << "\n";
    return EXIT_FAILURE;
  }

  return EXIT_SUCCESS;
}
