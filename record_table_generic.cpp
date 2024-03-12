/*
 *
 *
 */

#include "record_table_generic.h"

Record_table_generic::Record_table_generic(const Record_table_config & config,
        DUMP_STATE_CALLBACK dump_state) :
    m_config(config), m_dump_state(dump_state)
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

// Call a callback provided by client, so client can decide how to dump.
// If we returned the state to the client, we'd have to return a vector of
// states, if he wanted states of multiple tables.
void Record_table_generic::dump_state() const
{
    if (m_dump_state != nullptr) m_dump_state(*this);
}
