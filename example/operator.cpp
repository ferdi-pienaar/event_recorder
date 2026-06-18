/*
 *
 */

#include <iostream>
#include <string>
#include <sstream>
//#include "record_table_iterator.h"
#include "record_table_manager_itf.h"

// Pointer initialized by injection at initialization.
// xxx maybe just pass it from fn to fn, no need to save it here?
static const Record_table_manager_interface *table_mgr = nullptr;

static void handle_command();

// Note: this fn never returns.
void operator_init(const Record_table_manager_interface & mgr)
{
    table_mgr = &mgr;

    while (true)
    {
        handle_command();
    }
}

// Reading cin into a string and then parsing that may be clumsy, but it means I don't have to flush
// cin after each command.
// Example command: e t 0 (disable)
//                  s t 100 (set size 100)
//                  e t 1 (enable)
//                  d t (dump)
// xxx how do we make it that the 'name substring' can be nothing, to apply to all? Re-order? But param
// is also optional, so it also needs to be last?
void handle_command()
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
        table_mgr->enable_tables(name_ss, param);
        return;
    case 's': // size
        line >> param;
        std::cout << "size " << param << std::endl;
        table_mgr->size_tables(name_ss, param);
        return;
    case 'o': // oneshot/rollover
        line >> param;
        std::cout << (param ? "oneshot" : "rollover") << std::endl;
        table_mgr->oneshot_tables(name_ss, param);
        return;
    case 'd': // dump
        table_mgr->dump_tables(name_ss);
        return;
    case 'c': // clear
        table_mgr->clear_tables(name_ss);
        return;
    case 't': // tables state
        table_mgr->dump_tables_state(name_ss);
        return;
    default:
        std::cout << "unknown command: " << cmd << std::endl;
    }
}
