#include <iostream>
#include <monitor/src/ArgsOptions.hpp>

int main(int argc, char* argv[])
{
    monitor::ArgsOptions args(argc, argv);
    if (auto err = args.parseCommandLine(); err.has_value())
    {
        std::cerr << "Error: " << err->What() << std::endl;
        return -1;
    }

    std::cout << "config file=" << args.getConfigFile() << std::endl;

    std::string configFile = args.getConfigFile();
    return 0;
}