/*
 * Configuration that can be set at startup, and is contained in Record_table.
 * These properties may be changed by the client via the operator interface.
 *
 * xxx Is there any real purpose in making the data members private and accessing
 * via methods? The setters don't validate the data...
 */
#pragma once
#include "record_table_init_config.h"
#include <atomic>

class Record_table_config
{
public:
    // m_enabled is false if size=0 -- can't enable writing if there's no memory to write.
    Record_table_config(const Record_table_init_config &cfg)
        : m_size(cfg.m_size), m_enabled((cfg.m_size == 0) ? false : cfg.m_enabled),
          m_oneshot(cfg.m_oneshot)
    {
    }

    void set_size(unsigned s) noexcept { m_size = s; }
    void set_enabled(bool e) noexcept { m_enabled = e; }
    void set_oneshot(bool oneshot) noexcept { m_oneshot = oneshot; }

    unsigned get_size() const noexcept { return m_size; }
    bool get_enabled() const noexcept { return m_enabled; }
    bool get_oneshot() const noexcept { return m_oneshot; }

private:
    unsigned m_size = 0;
    // Enable is atomic because the operator sets it after preparing the table for writing;
    // after it is set, event-writing may proceed in another thread.
    std::atomic<bool> m_enabled = false;
    bool m_oneshot = false;
};
