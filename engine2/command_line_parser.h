#ifndef ENGINE2_COMMAND_LINE_PARSER_H_
#define ENGINE2_COMMAND_LINE_PARSER_H_

#include <map>
#include <string>
#include <vector>

namespace engine2 {

class CommandLineParser {
 public:
  CommandLineParser(int argc, char** argv);

  void Parse();

  std::string GetOwnProgramName() const;
  std::string GetFlag(const std::string& flag_name) const;
  bool HasFlag(const std::string& flag_name) const;

  bool CheckFlags(const std::vector<std::string>& flag_names) const;

  // Get an argument by its position (skipping named arguments that begin with
  // "==").
  std::string GetPositional(int index) const;
  bool HasPositional(int index) const;

  std::map<std::string, std::string>* flags() { return &flags_; }

 private:
  int argc_;
  char** argv_;

  std::map<std::string, std::string> flags_;
  std::vector<std::string> positional_;
};

}  // namespace engine2

#endif  // ENGINE2_COMMAND_LINE_PARSER_H_