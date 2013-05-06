#ifndef INCLUDED_CHIP_DESCRIPTION_H_
#define INCLUDED_CHIP_DESCRIPTION_H_

#include "base/base.h"
#include "chip/named_element.h"

#include <map>
#include <string>
#include <vector>

class ChipDescription : public NamedElement {
 public:
  typedef std::vector<std::string> StringArray;
  static ChipDescription* Create(std::string const & name,
                                 StringArray const & input_ports,
                                 StringArray const & output_ports);

  int num_input_ports() const { return input_port_names_.size(); }
  std::string const & input_port_name(int port) const {
    return input_port_names_[port];
  }

  int num_output_ports() const { return output_port_names_.size(); }
  std::string const & output_port_name(int port) const {
    return output_port_names_[port];
  }

  int has_input_port(std::string const & name) const {
    PortOffsetMap::const_iterator find(input_port_mapping_.find(name));
    if (find != input_port_mapping_.end()) {
      return find->second;
    }

    return -1;
  }

  int has_output_port(std::string const & name) const {
    PortOffsetMap::const_iterator find(output_port_mapping_.find(name));
    if (find != output_port_mapping_.end()) {
      return find->second;
    }

    return -1;
  }

 private:
  ChipDescription(std::string const & name,
                  StringArray const & input_ports,
                  StringArray const & output_ports);

  StringArray const input_port_names_;
  StringArray const output_port_names_;

  typedef std::map<std::string, int> PortOffsetMap;
  PortOffsetMap input_port_mapping_, output_port_mapping_;

  DISALLOW_COPY_AND_ASSIGN(ChipDescription);
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

  StringArray::const_iterator iter(input_port_names_.begin()),
    end(input_port_names_.end());
  StringArray::const_iterator begin(input_port_names_.begin());
  for (; iter != end; ++iter) {
    input_port_mapping_[*iter] = iter - begin;
  }

  iter = output_port_names_.begin();
  end = output_port_names_.end();
  begin = output_port_names_.begin();
  for (; iter != end; ++iter) {
    output_port_mapping_[*iter] = iter - begin;
  }
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
