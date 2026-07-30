/*
 * This does not check if data is valid; the checks are done when config is installed.
 */
#include "record_table_init_config.h"

namespace Event_record
{

// Static instance that contains the default values.
Table_init_config Table_init_config::CONFIG_DEFAULT;

Table_init_config &Table_init_config::size(unsigned s) noexcept
{
    m_size = s;
    return *this;
}

Table_init_config &Table_init_config::enable() noexcept
{
    m_enabled = true;
    return *this;
}

Table_init_config &Table_init_config::oneshot() noexcept
{
    m_oneshot = true;
    return *this;
}

} // namespace Event_record
