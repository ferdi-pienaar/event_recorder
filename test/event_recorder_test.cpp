/*
 */
#include <iostream>
#include "record_table.h"
#include "record_table_iterator.h"
#include "record_table_manager.h"
#include "gtest/gtest.h"
#include <array>
#include <vector>

class Record_table_default_config_test : public testing::Test
{
protected:
   Record_table_config config;
};

TEST_F(Record_table_default_config_test, default_members)
{
    EXPECT_EQ(0, config.get_size());
    EXPECT_FALSE(config.get_enabled());
}

// Can't enable unless size > 0.
TEST_F(Record_table_default_config_test, enable_no_entries)
{
    EXPECT_EQ(0, config.get_size());
    EXPECT_FALSE(config.get_enabled());
}

class Record_table_initialized_config_test : public testing::Test
{
protected:
   Record_table_config config = Record_table_config().oneshot().size(14).enable();
};

TEST_F(Record_table_initialized_config_test, verify_params)
{
    EXPECT_EQ(14, config.get_size());
    EXPECT_TRUE(config.get_enabled());
    EXPECT_TRUE(config.get_oneshot());
}

class Record_table_test : public testing::Test
{
protected:
};

TEST_F(Record_table_test, table_enabled)
{
    Record_table<int> rtable(Record_table_config().size(12).enable());

    EXPECT_TRUE(rtable.enabled());
}

// Can't modify size of an enabled table.
TEST_F(Record_table_test, set_size_enabled)
{
    Record_table<int> rtable(Record_table_config().size(1).enable());

    EXPECT_FALSE(rtable.set_size(2));
}

// Can't modify oneshot/rollover of an enabled table.
TEST_F(Record_table_test, set_oneshot_enabled)
{
    Record_table<int> rtable(Record_table_config().size(1).enable());

    EXPECT_FALSE(rtable.oneshot(true));
}

// Can't dump if no dump callback is registered.
TEST_F(Record_table_test, dump_no_callback)
{
    Record_table<int> rtable(Record_table_config().size(1));

    EXPECT_FALSE(rtable.dump());
}

// Can't dump_state if no dump_state callback is registered.
TEST_F(Record_table_test, dump_state_no_callback)
{
    Record_table<int> rtable(Record_table_config().size(1));

    EXPECT_FALSE(rtable.dump_state());
}

TEST_F(Record_table_test, table_enable_empty_fail)
{
    Record_table<int> rtable;

    EXPECT_FALSE(rtable.enable(true));
}

TEST_F(Record_table_test, table_enable)
{
    Record_table<int> rtable(Record_table_config().size(12));

    EXPECT_FALSE(rtable.enabled());
    EXPECT_TRUE(rtable.enable(true));
    EXPECT_TRUE(rtable.enabled());
}

TEST_F(Record_table_test, table_resize_and_enable)
{
    Record_table<int> rtable;

    EXPECT_TRUE(rtable.set_size(6));
    EXPECT_TRUE(rtable.enable(true));
    EXPECT_TRUE(rtable.enabled());
}

TEST_F(Record_table_test, write_1_entry)
{
    Record_table<int> rtable(Record_table_config().size(12).enable());

    rtable.get_write_entry() = 13;

    EXPECT_EQ(1, rtable.get_num_written_entries());
}

// A 'dumper' functor that lets test check what code-under-test dumps.
template <typename ENTRY>
struct Dump_spy
{
    // Save 'dumped' entries so we can examine them later.
    void operator()(const ENTRY & i)
    {
        store.push_back(i);
    }

    std::vector<ENTRY> store;
};

class Record_table_with_entries_test : public testing::Test
{
  protected:
    Record_table_with_entries_test()
        : itable(Record_table_config().size(12).enable(), std::ref(dump_spy)),
          table_iter(itable),
          empty_table(Record_table_config().size(12)),
          empty_table_iter(empty_table) {};

    virtual void SetUp()
    {
        itable.get_write_entry() = 5;
    }

    Dump_spy<int> dump_spy;
    Record_table<int> itable;
    Record_table_iterator<int> table_iter;
    Record_table<int> empty_table;
    Record_table_iterator<int> empty_table_iter;
};

TEST_F(Record_table_with_entries_test, iterator_first)
{
    table_iter.begin();
    EXPECT_FALSE(table_iter.end());
    EXPECT_EQ(5, table_iter.get_current());

    // After the single entry, reach the end.
    table_iter.next();
    EXPECT_TRUE(table_iter.end());
}

// Get write entry does not access an real entry because the table is disabled.
TEST_F(Record_table_with_entries_test, disabled_table_write)
{
    // Get write entry does not access an real entry because the table is disabled.
    empty_table.get_write_entry() = 13;

    empty_table_iter.begin();
    EXPECT_TRUE(empty_table_iter.end());
}

// Empty table is enabled, but still has no entries, so iterator ends.
TEST_F(Record_table_with_entries_test, iterator_no_writes)
{
    EXPECT_TRUE(empty_table.enable(true));
    empty_table_iter.begin();
    EXPECT_TRUE(empty_table_iter.end());
}

TEST_F(Record_table_with_entries_test, clear)
{
    EXPECT_TRUE(itable.enable(false));
    EXPECT_TRUE(itable.clear());
    // Verify no entries after clear.
    table_iter.begin();
    EXPECT_TRUE(table_iter.end());
}

TEST_F(Record_table_with_entries_test, iterator_next)
{
    itable.get_write_entry() = 13;

    table_iter.begin();
    EXPECT_FALSE(table_iter.end());
    EXPECT_EQ(5, table_iter.get_current());

    table_iter.next();
    EXPECT_FALSE(table_iter.end());
    EXPECT_EQ(13, table_iter.get_current());

    // After 2 written entries, reach the end.
    table_iter.next();
    EXPECT_TRUE(table_iter.end());
}

// Some different ways 'record' clients can write and overwrite entries.
TEST_F(Record_table_with_entries_test, table_overwrite)
{
    auto & e1 = itable.get_write_entry(); // advance
    e1 = 13;
    e1 = 14; // overwrite
    itable.get_write_entry(false) = 15; // no advance, so next line overwrites.
    itable.get_write_entry() = 16; // overwrite
    itable.get_write_entry(false) = 17;
    itable.done(); // advance, so next line does not overwrite.
    itable.get_write_entry() = 18;

    EXPECT_TRUE(itable.dump());
    ASSERT_EQ(5, dump_spy.store.size());
    EXPECT_EQ(5, dump_spy.store[0]);
    EXPECT_EQ(14, dump_spy.store[1]);
    EXPECT_EQ(16, dump_spy.store[2]);
    EXPECT_EQ(17, dump_spy.store[3]);
    EXPECT_EQ(18, dump_spy.store[4]);
}

TEST_F(Record_table_with_entries_test, rollover)
{
    // Reduce size to trigger rollover.
    EXPECT_TRUE(itable.enable(false));
    EXPECT_TRUE(itable.set_size(1));
    EXPECT_TRUE(itable.enable(true));

    itable.get_write_entry() = 25;
    itable.get_write_entry() = 29;
    
    EXPECT_TRUE(itable.dump());
    ASSERT_EQ(1, dump_spy.store.size());
    EXPECT_EQ(29, dump_spy.store[0]);
}

TEST_F(Record_table_with_entries_test, oneshot)
{
    EXPECT_TRUE(itable.enable(false));
    EXPECT_TRUE(itable.set_size(2));
    EXPECT_TRUE(itable.oneshot(true));
    EXPECT_TRUE(itable.enable(true));

    itable.get_write_entry() = 13;
    EXPECT_TRUE(itable.active());
    itable.get_write_entry() = 14;
    EXPECT_FALSE(itable.active());
    itable.get_write_entry() = 15;

    // Only the first 2 entries are saved, because table is full.
    EXPECT_TRUE(itable.dump());
    ASSERT_EQ(2, dump_spy.store.size());
    EXPECT_EQ(13, dump_spy.store[0]);
    EXPECT_EQ(14, dump_spy.store[1]);
}

// After changing from oneshot to rollover, table records new entries.
TEST_F(Record_table_with_entries_test, oneshot_to_rollover)
{
    EXPECT_TRUE(itable.enable(false));
    EXPECT_TRUE(itable.set_size(2));
    EXPECT_TRUE(itable.oneshot(true));
    EXPECT_TRUE(itable.enable(true));

    itable.get_write_entry() = 13;
    itable.get_write_entry() = 14;
    EXPECT_FALSE(itable.active()); // stopped.

    // Disable, change to rollover mode, and re-enable.
    EXPECT_TRUE(itable.enable(false));
    EXPECT_TRUE(itable.oneshot(false));
    EXPECT_TRUE(itable.enable(true));

    // Table is active and adds new entry.
    EXPECT_TRUE(itable.active()); // no longer stopped.
    itable.get_write_entry() = 15;

    // Only most recent entries are saved due to rollover.
    EXPECT_TRUE(itable.dump());
    ASSERT_EQ(2, dump_spy.store.size());
    EXPECT_EQ(14, dump_spy.store[0]);
    EXPECT_EQ(15, dump_spy.store[1]);
}

// A 'dumper' functor that lets test check what code-under-test dumps.
class Record_table_state_spy
{
public:
    // Save 'dumped' data so we can examine it later.
    void operator()(const Record_table_op_itf & table)
    {
        called = true;
        enabled = table.enabled();
        oneshot = table.oneshot();
        size = table.size();
        stopped = table.is_stopped();
    }

    bool called = false; // has this functor been called?
    bool enabled = false;
    bool oneshot = false;
    unsigned size = 0;
    bool stopped = false;
};

// A 'dumper' functor that lets test check what table names manager dumps.
struct Name_spy
{
    // Save 'dumped' names so we can examine them later.
    void operator()(const std::string &name)
    {
        names.push_back(name);
    }

    std::vector<std::string> names;
};

// A manager with several tables, some of them enabled and with entries.
class Record_table_manager_test : public testing::Test
{
  protected:
    Record_table_manager_test()
        : itable(Record_table_config().size(12), std::ref(int_dump_spy)),
          ftable(Record_table_config().size(13).enable(), std::ref(float_dump_spy)),
          ftable2(Record_table_config().size(14).oneshot().enable(), std::ref(float_dump_spy2),
                  std::ref(state_spy)),
          ftable3(Record_table_config().size(15).oneshot().enable(), std::ref(float_dump_spy3)),
          mgr({{"int-table", itable},
               {"float-table", ftable},
               {"float-table2", ftable2},
               {"ftab3", ftable3}},
              std::ref(name_spy)) {};

    virtual void SetUp()
    {
        ftable.get_write_entry() = 2.5;
        ftable2.get_write_entry() = 3.5;
        ftable3.get_write_entry() = 5.5;
    }

    Record_table_state_spy state_spy;
    Dump_spy<int> int_dump_spy;
    Record_table<int> itable;
    Dump_spy<float> float_dump_spy;
    Record_table<float> ftable;
    Dump_spy<float> float_dump_spy2;
    Record_table<float> ftable2;
    Dump_spy<float> float_dump_spy3;
    Record_table<float> ftable3;
    Name_spy name_spy;
    Record_table_manager mgr;
};

// Verify disabled table is enabled by a call to its manager.
TEST_F(Record_table_manager_test, enable)
{
    EXPECT_TRUE(mgr.enable_tables("int-tab", true));
    EXPECT_TRUE(itable.enabled());
}

// Verify disabled table is set oneshot by a call to its manager.
TEST_F(Record_table_manager_test, oneshot)
{
    EXPECT_TRUE(mgr.oneshot_tables("int-tab", true));
    EXPECT_TRUE(itable.oneshot());
}

// Verify table size is changed by a call to its manager.
TEST_F(Record_table_manager_test, set_size)
{
    EXPECT_TRUE(mgr.size_tables("int-tab", 14));
    EXPECT_EQ(14, itable.size());
}

// Verify table entries are cleared by a call to its manager.
TEST_F(Record_table_manager_test, clear)
{
    EXPECT_EQ(1, ftable.get_num_written_entries());

    // Must disable the table for writing before we clear it.
    EXPECT_TRUE(mgr.enable_tables("float-table", false));
    EXPECT_TRUE(mgr.clear_tables("float-table"));
    EXPECT_EQ(0, ftable.get_num_written_entries());
}

TEST_F(Record_table_manager_test, dump)
{
    EXPECT_TRUE(mgr.dump_tables("table"));

    ASSERT_EQ(1, float_dump_spy.store.size());
    EXPECT_EQ(2.5, float_dump_spy.store.front());
}

// Manager dumps data from more than 1 table that it owns, but only for the tables that match the substring.
TEST_F(Record_table_manager_test, mgr_match2)
{
    EXPECT_TRUE(mgr.dump_tables("table"));

    // Nothing dumped by this table because it is empty.
    EXPECT_EQ(0, int_dump_spy.store.size());

    ASSERT_EQ(1, float_dump_spy.store.size());
    EXPECT_EQ(2.5, float_dump_spy.store.front());

    ASSERT_EQ(1, float_dump_spy2.store.size());
    EXPECT_EQ(3.5, float_dump_spy2.store.front());

    // Nothing dumped by this table because its name does not match 'table' substring.
    EXPECT_EQ(0, float_dump_spy3.store.size());
}

// Verify that record table responds to its manager's dump_state request.
TEST_F(Record_table_manager_test, dump_state)
{
    EXPECT_TRUE(mgr.dump_tables_state("float-table2"));

    ASSERT_TRUE(state_spy.called);
    EXPECT_TRUE(state_spy.enabled);
    EXPECT_TRUE(state_spy.oneshot);
    EXPECT_EQ(14, state_spy.size);
    EXPECT_FALSE(state_spy.stopped);
}

// Fail because some of the named tables don't have a registered dump-state callback.
TEST_F(Record_table_manager_test, dump_state_fail)
{
    EXPECT_FALSE(mgr.dump_tables_state("tab"));
}

// As a side-effect of doing other operation, mgr outputs the names of the tables that sub-string.
TEST_F(Record_table_manager_test, names)
{
    EXPECT_TRUE(mgr.dump_tables("f"));

    ASSERT_EQ(3, name_spy.names.size());
    EXPECT_EQ("float-table", name_spy.names[0]);
    EXPECT_EQ("float-table2", name_spy.names[1]);
    EXPECT_EQ("ftab3", name_spy.names[2]);
}
