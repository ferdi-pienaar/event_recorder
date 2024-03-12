/*
 *
 *
 */

#include "record_table_config.h"

// Static instance that contains the default values.
Record_table_config Record_table_config::CONFIG_DEFAULT;

Record_table_config & Record_table_config::size(unsigned s)
{
    m_size = s;
    return *this;
}

Record_table_config & Record_table_config::enable()
{
    if (m_size > 0)
    {
        m_enabled = true;
    }
    else
    {
        // xxx WARNING can't enable writing if number of entries is 0.
    }
    return *this;
}

Record_table_config & Record_table_config::oneshot()
{
    m_oneshot = true;
    return *this;
}
