#include <iostream>

#include "engine2/command_line_parser.h"

namespace engine2 {
namespace {

bool IsFlag(const char* val) {
  return val[0] == '-' && val[1] == '-';
}

std::pair<std::string, std::string> GetFlagNameAndValue(const char* val) {
  // Skip leading dashes
  std::string str(val + 2);
  size_t len = str.size();
  size_t separator_pos = str.find('=');
  if (separator_pos == std::string::npos)
    return {str, {}};

  if (separator_pos == len - 1)
    return {str.substr(0, separator_pos), {}};

  return {str.substr(0, separator_pos), str.substr(separator_pos + 1)};
}

}  // namespace

CommandLineParser::CommandLineParser(int argc, char** argv)
    : argc_(argc), argv_(argv) {}

void CommandLineParser::Parse() {
  for (int i = 1; i < argc_; ++i) {
    if (IsFlag(argv_[i])) {
      flags_.insert(GetFlagNameAndValue(argv_[i]));
    } else {
      positional_.emplace_back(argv_[i]);
    }
  }
}

std::string CommandLineParser::GetOwnProgramName() const {
  return argv_[0];
}

std::string CommandLineParser::GetFlag(const std::string& flag_name) const {
  auto iter = flags_.find(flag_name);
  if (iter == flags_.end())
    return {};

  return iter->second;
}

bool CommandLineParser::HasFlag(const std::string& flag_name) const {
  return flags_.find(flag_name) != flags_.end();
}

bool CommandLineParser::CheckFlags(
    const std::vector<std::string>& flag_names) const {
  bool has_all_flags = true;
  for (auto& flag_name : flag_names) {
    if (!HasFlag(flag_name)) {
      has_all_flags = false;
      std::cerr << "Missing required flag '" << flag_name << "'.\n";
    }
  }
  return has_all_flags;
}

std::string CommandLineParser::GetPositional(int index) const {
  if (!HasPositional(index))
    return {};

  return positional_[index];
}

bool CommandLineParser::HasPositional(int index) const {
  return index >= 0 && index < positional_.size();
}

}  // namespace engine2