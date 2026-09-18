/*
 */
#pragma once
#include <functional>
#include <sstream>
#include <string>

namespace Event_record
{
class Table_manager_interface;
}

class Operator
{
public:
    Operator(const Event_record::Table_manager_interface &mgr) : m_mgr(mgr) {}
    void run() const;

private:
    bool handle_command() const;
    bool handle_command(const std::string &cmd, std::istringstream &line,
                        const std::function<bool(const std::string &)> &matcher) const;

    const Event_record::Table_manager_interface &m_mgr;
};
