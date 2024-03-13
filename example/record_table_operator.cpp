/*
 *
 */

#include <iostream>
#include <string>
#include "record_table.h"
#include "record_table_iterator.h"
#include "record_table_manager.h"
#include "record_table_types.h"
#include "record_table_defs.h"
#include "record_table_recorder.h"
#include "record_table_helper.h"

static void handle_command();
static void parse(char cmd, std::string name, unsigned param);

int main(int argc, char * argv[])
{
    while (true)
    {
        handle_command();
    }
    return 0;
}

void handle_command()
{
    std::cout << "handle_command" << std::endl;
    char cmd;
    std::string name;
    unsigned param;

    std::cin >> cmd >> name >> param;

    parse(cmd, name, param);

    // Flush input buffer? Apparently not.
    std::cin.clear();
}

Record_table_manager mgr(
{
    {"time", get_time_record_table()},
    {"integer", get_int_record_table()}
},
dump_name_cb);

void parse(char cmd, std::string name, unsigned param)
{
    std::cout << cmd << " " << name << " " << param << std::endl;

    switch (cmd)
    {
    case 'e': // enable/disable
        mgr.enable_tables(name, param);
        break;
    case 's': // size
        mgr.size_tables(name, param);
        break;
    case 'o': // oneshot/rollover
        mgr.oneshot_tables(name, param);
        break;
    case 'd': // dump
        mgr.dump_tables(name);
        break;
    case 'c': // clear
        mgr.clear_tables(name);
        break;
    case 't': // tables state
        mgr.dump_tables_state(name);
        break;
    case 'r': // record event(s).
        if (name == "time")
        {
            time_event();
        }
        else
        {
            int_event();
        }
        break;

    default:
        std::cout << "unknown command" << std::endl;
    }
}
