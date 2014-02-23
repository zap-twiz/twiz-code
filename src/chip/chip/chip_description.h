#ifndef INCLUDED_CHIP_DESCRIPTION_H_
#define INCLUDED_CHIP_DESCRIPTION_H_

#include "named_element.h"

#include <string>
#include <vector>

class ChipDescription : public NamedElement {
 public:
  typedef std::vector<std::string> StringArray;
  static ChipDescription* Create(std::string const & name,
                                 StringArray const & input_ports,
                                 StringArray const & output_orts);

  int num_input_ports() const { return input_port_names_.size(); }
  std::string const & input_port_name(int port) const {
    return input_port_names_[port];
  }

  int num_output_ports() const { return output_port_names_.size(); }
  std::string const & output_port_name(int port) const {
    return output_port_names_[port];
  }

 private:
  ChipDescription(std::string const & name,
                  StringArray const & input_ports,
                  StringArray const & output_ports);

  StringArray const input_port_names_;
  StringArray const output_port_names_;
};

ChipDescription* ChipDescription::Create(std::string const & name,
                                         StringArray const & input_ports,
                                         StringArray const & output_ports) {
  return new ChipDescription(name, input_ports, output_ports);
}

ChipDescription::ChipDescription(std::string const & name,
                                 StringArray const & input_ports,
                                 StringArray const & output_ports)
    : NamedElement(name),
      input_port_names_(input_ports),
      output_port_names_(output_ports) {
}

#include <sstream>

// Helper routine
ChipDescription::StringArray CreateStrings(std::string const & prefix, int n) {
  ChipDescription::StringArray outputs;
  for (int x = 0; x < n; ++x) {
    std::ostringstream s;
    s << prefix << x;
    outputs.push_back(s.str());
  }
  return outputs;
}

#endif // INCLUDED_CHIP_DESCRIPTION_H_
