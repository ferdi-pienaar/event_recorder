/*
 * This enforces the rule that a config with size 0 may not be enabled. This avoids writing to
 * non-existent entries. It is enforced here to avoid clients creating an invalid config and passing
 * it to a Record_table when the Record_table is created. Other rules, such as not modifying configs
 * while enabled, are enforced elsewhere.
 */
#include "record_table_config.h"

// Static instance that contains the default values.
Record_table_config Record_table_config::CONFIG_DEFAULT;

Record_table_config &Record_table_config::size(unsigned s) noexcept
{
    m_size = s;
    return *this;
}

Record_table_config & Record_table_config::enable() noexcept
{
    auto result = set_enabled(true);
    if (result == false)
    {
        // xxx issue a warning?
    }
    return *this;
}

Record_table_config & Record_table_config::oneshot() noexcept
{
    m_oneshot = true;
    return *this;
}

bool Record_table_config::set_enabled(bool ena) noexcept
{
    if (ena && m_size == 0)
    {
        // Not allowed to enable if size is 0.
        return false;
    }
    m_enabled = ena;
    return true;
}
