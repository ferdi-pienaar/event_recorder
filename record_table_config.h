/*
 *
 *
 */

#pragma once

// Configuration that can be set at startup, and is contained in Record_table.
// Some of these properties may be modified later by the client, others are fixed.
class Record_table_config
{
public:
    Record_table_config & size(unsigned s);
    Record_table_config & enable();
    Record_table_config & oneshot();

    unsigned m_size = 0;
    bool m_enabled = false;
    bool m_oneshot = false;

    static Record_table_config CONFIG_DEFAULT;
};
