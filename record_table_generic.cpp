/*
 *
 *
 */

#include "record_table_generic.h"
#include <iostream>

Record_table_generic::Record_table_generic(const Record_table_config & config) : m_config(config)
{
}

bool Record_table_generic::enabled() const
{
    return m_config.m_enabled;
}

// Note that if client changes to one-shot, existing data is not cleared, so new
// data will only be gathered until the array is full. If it is already full, no new
// data will be gathered. If client wants to gather new data, it must call clear, before
// or after switching to one-shot mode.
bool Record_table_generic::oneshot(bool mode)
{
    if (m_config.m_enabled)
    {
        return false;
    }

    m_config.m_oneshot = mode;
    return true;
}

bool Record_table_generic::active() const
{
    return m_config.m_enabled && !m_stopped;
}
