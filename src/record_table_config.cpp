/*
 * This does not check if data is valid; the checks are done when config is installed.
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
    m_enabled = true;
    return *this;
}

Record_table_config & Record_table_config::oneshot() noexcept
{
    m_oneshot = true;
    return *this;
}
