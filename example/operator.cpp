/*
 * This is the client's command-line interface to a human operator.
 * It displays the data created by the event generator.
 * It depends only on the abstract interface record_table_manager_itf.h.
 */
#include "operator.h"
#include <iostream>
#include <string>
#include <sstream>
#include "record_table_manager_itf.h"

// Note: this fn never returns.
void Operator::init(const Record_table_manager_interface & mgr)
{
    while (true)
    {
        if (handle_command(mgr) == false)
        {
            std::cout << "Command failed on one or more tables." << std::endl;
        }
    }
}

// Read line from stdin and make corresponding calls to Table Manager.
// Example command: e t 0 (disable)
//                  s t 100 (set size 100)
//                  e t 1 (enable)
//                  d t (dump)
// Reading cin into a string and then parsing that may be clumsy, but it means I don't have to flush
// cin after each command.
// xxx how do we make it that the 'name substring' can be nothing, to apply to all? Re-order? But param
// is also optional, so it also needs to be last?
bool Operator::handle_command(const Record_table_manager_interface & table_mgr)
{
    std::cout << "handle_command: enter ['e'|'s'|'o'|'d'|'c'|'t'] [name substring] [int param for e, s, o]" << std::endl;

    static const Record_table_manager_interface *mgr = nullptr;
    std::string input;
    std::getline(std::cin, input);
    std::istringstream line(input);

    char cmd;
    std::string name_ss;
    line >> cmd >> name_ss;
    std::cout << "cmd '" << cmd << "' name sub-string '" << name_ss << "'" << std::endl;

    unsigned param;
    switch (cmd)
    {
    case 'e': // enable/disable
        line >> param;
        std::cout << (param ? "enable" : "disable") << std::endl;
        return table_mgr.enable_tables(name_ss, param);
    case 's': // size
        line >> param;
        std::cout << "size " << param << std::endl;
        return table_mgr.size_tables(name_ss, param);
    case 'o': // oneshot/rollover
        line >> param;
        std::cout << (param ? "oneshot" : "rollover") << std::endl;
        return table_mgr.oneshot_tables(name_ss, param);
    case 'd': // dump
        return table_mgr.dump_tables(name_ss);
    case 'c': // clear
        return table_mgr.clear_tables(name_ss);
    case 't': // tables state
        return table_mgr.dump_tables_state(name_ss);
    default:
        std::cout << "unknown command: " << cmd << std::endl;
    }
}
