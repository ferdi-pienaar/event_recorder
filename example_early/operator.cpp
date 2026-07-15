/*
 * This is the client's command-line interface to a human operator.
 * It displays the data created by the event generator.
 * It depends only on the abstract interface record_table_manager_itf.h. It is independent of
 * the number and type of Event_record::Tables owned by its Event_record::Table_manager.
 */
#include "operator.h"
#include "record_table_manager_itf.h"
#include <iostream>
#include <sstream>
#include <string>

// Note: this fn never returns.
void Operator::run()
{
    while (true)
    {
        if (handle_command() == false)
        {
            std::cout << "Command failed on one or more tables." << std::endl;
        }
    }
}

// Read line from stdin and make corresponding calls to Table Manager.
// Example commands: "enable": enable all tables
//                   "e": enable all tables
//                   "si 10": set all tables to size 10.
//                   "si 12 ti": set to size 12 the tables with names including string "ti"
// You may use only the initial letters of each command, but must write at least "si" to
// distinguish "size" from "state" and "di" to distinguish "disable" from "dump".
// Reading cin into a string and then parsing that may be clumsy, but it means I don't have to flush
// cin after each command.
bool Operator::handle_command()
{
    std::cout << "handle_command: dump, enable, disable, state, size, one, roll, clear"
              << std::endl;

    std::string input;
    std::getline(std::cin, input);
    std::istringstream line(input);

    std::string cmd;
    line >> cmd;
    std::string name_ss;
    if (std::string("dump").rfind(cmd, 0) == 0) // Before "disable", so "d" is "dump" not "disable".
    {
        line >> name_ss;
        return m_mgr.dump_tables(name_ss);
    }
    if (std::string("state").rfind(cmd, 0) == 0) // Before "size", so "s" is "state" not "size".
    {
        line >> name_ss;
        return m_mgr.dump_tables_state(name_ss);
    }
    if (std::string("enable").rfind(cmd, 0) == 0)
    {
        line >> name_ss;
        return m_mgr.enable_tables(name_ss, true);
    }
    if (std::string("disable").rfind(cmd, 0) == 0)
    {
        line >> name_ss;
        return m_mgr.enable_tables(name_ss, false);
    }
    if (std::string("oneshot").rfind(cmd, 0) == 0)
    {
        line >> name_ss;
        return m_mgr.oneshot_tables(name_ss, true);
    }
    if (std::string("rollover").rfind(cmd, 0) == 0)
    {
        line >> name_ss;
        return m_mgr.oneshot_tables(name_ss, false);
    }
    if (std::string("clear").rfind(cmd, 0) == 0)
    {
        line >> name_ss;
        return m_mgr.clear_tables(name_ss);
    }
    if (std::string("size").rfind(cmd, 0) == 0)
    {
        unsigned size;
        line >> size >> name_ss;
        return m_mgr.size_tables(name_ss, size);
    }
    else
    {
        std::cout << "unknown command: " << cmd << std::endl;
        return false;
    }
}
