/*
 *
 *
 */

#pragma once

// Configuration that can be set at startup, and is contained in Record_table.
// Some of these properties may be modified later by the client, others are fixed.
// The methods return a reference to the struct so that calls can be chained together.
// In such a chain, size() should be called before enable(), since a table with the default
// size of 0 cannot be enabled.
struct Record_table_config
{
    Record_table_config & size(unsigned s) noexcept;
    Record_table_config & enable() noexcept;
    Record_table_config & oneshot() noexcept;

    unsigned m_size = 0;
    bool m_enabled = false;
    bool m_oneshot = false;

    static Record_table_config CONFIG_DEFAULT;
};
