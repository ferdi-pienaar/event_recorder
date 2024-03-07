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
