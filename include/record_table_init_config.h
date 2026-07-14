/*
 * Configuration used to intialize Event_record::Table at creation.
 * These properties may be changed by the client via the operator interface.
 * The methods exist largely for cosmetic reasons; they allow modifying the
 * default values in a readable way.
 */
#pragma once

namespace Event_record
{

// Data members are public; data validity is checked when it is installed.
class Table_init_config
{
public:
    // These methods return a reference to the struct so that calls can be chained together.
    Table_init_config &size(unsigned s) noexcept;
    Table_init_config &enable() noexcept;
    Table_init_config &oneshot() noexcept;

    static Table_init_config CONFIG_DEFAULT;

    unsigned m_size = 0;
    bool m_enabled = false;
    bool m_oneshot = false;
};

} // namespace Event_record
