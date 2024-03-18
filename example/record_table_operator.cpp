/*
 *
 */

#include <iostream>
#include <string>
#include "record_table.h"
#include "record_table_iterator.h"
#include "record_table_manager.h"
#include "record_table_types.h"
#include "record_table_recorder.h"
#include "record_table_helper.h"

static void handle_command();
static void parse(const std::string & domain, char cmd, const std::string & name, unsigned param);

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

    std::string domain;
    char cmd;
    std::string name;
    unsigned param;

    std::cin >> domain >> cmd >> param >> name;

    parse(domain, cmd, name, param);

    // Flush input buffer? Apparently not.
    std::cin.clear();
}

static Record_table<Double_stamp> ttable(Record_table_config().size(NUM_ENTRIES).enable(), dump_ts_array_cb, dump_table_state_cb);
Record_table_manager<Double_stamp> & get_timestamp_manager()
{
    static Record_table_manager<Double_stamp> mgr({{"time1", ttable}}, dump_name_cb);
    return mgr;
}

static Record_table<int> itable(Record_table_config().size(NUM_ENTRIES).enable(), dump_int_cb, dump_table_state_cb);
Record_table_manager<int> & get_int_manager()
{
    static Record_table_manager<int> mgr({{"int1", itable}}, dump_name_cb);
    return mgr;
}

static std::map<std::string, Record_table_manager_interface &> managers = {{"time", get_timestamp_manager()}, {"int", get_int_manager()}};

void parse(const std::string & domain, char cmd, const std::string & name, unsigned param)
{
    std::cout << "domain: '" << domain << "' cmd " << cmd << " name '" << name << "' param " << param << std::endl;

    if (domain == "time")
    {
        get_timestamp_manager().handle_cmd(cmd, name, param);
    }
    else if (domain == "int")
    {
        get_int_manager().handle_cmd(cmd, name, param);
    }
    else if (domain == "event")
    {
        if (name == "time")
        {
            time_event();
        }
        else
        {
            int_event();
        }
    }
}
