/*
 * Configuration that can be set at startup, and is passed to Record_table.
 * These properties may be changed by the client via the operator interface.
 * The methods exist largely for cosmetic reasons; they allow modifying the
 * default values in a readable way.
 */
#pragma once

// Data members are public; data validity is checked when it is installed.
class Record_table_config
{
public:
    // These methods return a reference to the struct so that calls can be chained together.
    Record_table_config & size(unsigned s) noexcept;
    Record_table_config & enable() noexcept;
    Record_table_config & oneshot() noexcept;

    static Record_table_config CONFIG_DEFAULT;

    unsigned m_size = 0;
    bool m_enabled = false;
    bool m_oneshot = false;
};
