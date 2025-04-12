#pragma once
#include <database/Define.hpp>
#include <boost/program_options.hpp>

namespace service::database
{

class ArgsOptions
{
public:
    ArgsOptions(int argc, char* argv[]);
    ~ArgsOptions() = default;

    ErrOpt parseCommandLine();
    std::string getConfigFile() const;

private:    
    void PrintHelp() const;
    void PrintVersion() const;
private:
    int m_argc;
    char** m_argv;
    boost::program_options::options_description m_desc;
    boost::program_options::variables_map m_vm;
};

} // namespace monitor