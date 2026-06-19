/*
 *
 */
#include "record_table_config.h"

// Static instance that contains the default values.
Record_table_config Record_table_config::CONFIG_DEFAULT;

// xxx do we allow setting size while enabled? Can we put the check here instead of in Record_table?
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
