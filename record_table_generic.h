/*
 *
 *
 */

#pragma once
#include "record_table_config.h"

// Code that's not specific to the type of entry in derived Record_table class template.
class Record_table_generic
{
public:
    Record_table_generic(const Record_table_config & );
    bool enabled() const;
    bool oneshot(bool);
    bool active() const;

protected:
    Record_table_config m_config;
    unsigned m_num_advances = 0; // Number of write advances, capped at config.size.
    bool m_stopped = false; // One-shot full => true, clear => false. xxx could also be in Record_table.
};
