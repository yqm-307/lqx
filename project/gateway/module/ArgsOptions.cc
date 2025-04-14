#include <iostream>
#include <gateway/module/ArgsOptions.hpp>

namespace service::gateway
{

const std::string desc = R"(
===================
GateWay Service
Usage:
    gateway -c <config_file>)";

ArgsOptions::ArgsOptions(int argc, char* argv[])
    : m_argc(argc), m_argv(argv), m_desc(desc)
{
    m_desc.add_options()
        ("help,h", "Show help message")
        ("version,v", "Show version information")
        ("config,c", boost::program_options::value<std::string>(), "Path to the configuration file");
}

ErrOpt ArgsOptions::parseCommandLine()
{
    try
    {
        boost::program_options::store(boost::program_options::parse_command_line(m_argc, m_argv, m_desc), m_vm);
        boost::program_options::notify(m_vm);
    }
    catch(const boost::program_options::error& e)
    {
        return Errcode{e.what(), emErr::ERR_PARSE_CMDLINE_FAILED};
    }

    if (m_vm.count("help"))
    {
        PrintHelp();
        return std::nullopt;
    }

    if (m_vm.count("version"))
    {
        PrintVersion();
        return std::nullopt;
    }

    if (m_vm.count("config"))
    {
        std::string configFile = m_vm["config"].as<std::string>();
        return std::nullopt;
    }
    else
    {
        std::cerr << "Error: Configuration file is required." << std::endl;
        PrintHelp();
        return Errcode{"Configuration file is required", emErr::ERR_PARSE_CMDLINE_FAILED};
    }

    return std::nullopt;
}

void ArgsOptions::PrintHelp() const
{
    std::cout << m_desc << std::endl;
}

void ArgsOptions::PrintVersion() const
{
    std::cout << "Monitor version 1.0" << std::endl;
}

std::string ArgsOptions::getConfigFile() const
{
    if (m_vm.count("config"))
    {
        return m_vm["config"].as<std::string>();
    }
    return "";
}

} // namespace monitor