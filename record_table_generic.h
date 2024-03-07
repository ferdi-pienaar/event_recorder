/*
 *
 *
 */

#pragma once
#include "record_table_config.h"

class Record_table_generic
{
public:
    Record_table_generic(const Record_table_config & );
    bool enabled() const;

protected:
    Record_table_config m_config;
    unsigned m_num_advances = 0; // Number of write advances, capped at config.size.
};
