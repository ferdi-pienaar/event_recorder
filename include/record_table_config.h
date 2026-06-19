/*
 * Configuration that can be set at startup, and is contained in Record_table.
 * These properties may be changed by the client via the operator interface.
 */
#pragma once

// Data members are private so clients can't set up invalid config like an enabled table with size
// 0, or perform actions like changing size or oneshot while enabled.
class Record_table_config
{
public:
    // These methods return a reference to the struct so that calls can be chained together.
    // In such a chain, call size() before enable(), since a table with the default
    // size of 0 cannot be enabled.
    Record_table_config & size(unsigned s) noexcept;
    Record_table_config & enable() noexcept;
    Record_table_config & oneshot() noexcept;

    // xxx should we check here if enabled and return error?
    void set_size(unsigned s) noexcept { m_size = s; }
    bool set_enabled(bool) noexcept;
    // xxx should we check here if enabled and return error?
    void set_oneshot(bool oneshot) noexcept { m_oneshot = oneshot; }

    unsigned get_size() const noexcept { return m_size; }
    bool get_enabled() const noexcept { return m_enabled; }
    bool get_oneshot() const noexcept { return m_oneshot; }
    static Record_table_config CONFIG_DEFAULT;

private:
    unsigned m_size = 0;
    bool m_enabled = false;
    bool m_oneshot = false;
};
