#pragma once
#include <boost/program_options.hpp>

#include <monitor/src/Define.hpp>

namespace monitor
{

class ArgsOptions
{
public:
    ArgsOptions(int argc, char* argv[]);
    ~ArgsOptions() = default;

    ErrOpt parseCommandLine();
    std::string getConfigFile() const;

private:    
    void printHelp() const;
    void printVersion() const;
private:
    int m_argc;
    char** m_argv;
    boost::program_options::options_description m_desc;
    boost::program_options::variables_map m_vm;
};

}