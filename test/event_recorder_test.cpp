/*
 *
 */

#include <iostream>
#include "record_table.h"
#include "record_table_iterator.h"
#include "record_table_manager.h"
#include "gtest/gtest.h"
#include <array>
#include <time.h> // struct timespec

class Record_table_test : public testing::Test
{
};

TEST_F(Record_table_test, default_config)
{
    Record_table_config c;
    EXPECT_EQ(0, c.get_size());
    EXPECT_FALSE(c.get_enabled());
}

// Can't enable unless size > 0.
TEST_F(Record_table_test, config_enable_no_entries)
{
    Record_table_config c = Record_table_config().enable();
    EXPECT_EQ(0, c.get_size());
    EXPECT_FALSE(c.get_enabled());
}

TEST_F(Record_table_test, config_size_enable)
{
    Record_table_config c = Record_table_config().size(14).enable();
    EXPECT_EQ(14, c.get_size());
    EXPECT_TRUE(c.get_enabled());
}

TEST_F(Record_table_test, table_enabled)
{
    Record_table<int> rtable(Record_table_config().size(12).enable());

    EXPECT_TRUE(rtable.enabled());
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

TEST_F(Record_table_test, iterator_first)
{
    Record_table<int> rtable(Record_table_config().size(12).enable());

    rtable.get_write_entry() = 13;

    Record_table_iterator<int> table_iter(rtable);
    table_iter.begin();
    EXPECT_FALSE(table_iter.end());
    EXPECT_EQ(13, table_iter.get_current());

    // After the single entry, reach the end.
    table_iter.next();
    EXPECT_TRUE(table_iter.end());
}

TEST_F(Record_table_test, table_size_0_write)
{
    Record_table<int> rtable;
    rtable.get_write_entry() = 13;

    Record_table_iterator<int> table_iter(rtable);
    table_iter.begin();
    EXPECT_TRUE(table_iter.end());
}

TEST_F(Record_table_test, table_disabled_write)
{
    Record_table<int> rtable(Record_table_config().size(12));
    rtable.get_write_entry() = 13;

    Record_table_iterator<int> table_iter(rtable);
    table_iter.begin();
    EXPECT_TRUE(table_iter.end());
}

TEST_F(Record_table_test, iterator_no_writes)
{
    Record_table<int> rtable(Record_table_config().size(12).enable());

    Record_table_iterator<int> table_iter(rtable);
    table_iter.begin();
    EXPECT_TRUE(table_iter.end());
}

TEST_F(Record_table_test, clear)
{
    Record_table<int> rtable(Record_table_config().size(12).enable());
    rtable.get_write_entry() = 13;

    Record_table_iterator<int> table_iter(rtable);
    table_iter.begin();
    EXPECT_FALSE(table_iter.end());

    rtable.enable(false);
    EXPECT_TRUE(rtable.clear());
    // Verify no entries after clear.
    table_iter.begin();
    EXPECT_TRUE(table_iter.end());
}

TEST_F(Record_table_test, iterator_next)
{
    Record_table<int> rtable(Record_table_config().size(12).enable());
    rtable.get_write_entry() = 13;
    rtable.get_write_entry() = 14;

    Record_table_iterator<int> table_iter(rtable);
    table_iter.begin();
    EXPECT_FALSE(table_iter.end());
    EXPECT_EQ(13, table_iter.get_current());

    table_iter.next();
    EXPECT_FALSE(table_iter.end());
    EXPECT_EQ(14, table_iter.get_current());

    // After 2 written entries, reach the end.
    table_iter.next();
    EXPECT_TRUE(table_iter.end());
}

// Some different ways 'record' clients can write and overwrite entries.
TEST_F(Record_table_test, table_overwrite)
{
    Record_table<int> rtable(Record_table_config().size(12).enable());
    auto & e1 = rtable.get_write_entry(); // advance
    e1 = 13;
    e1 = 14; // overwrite
    rtable.get_write_entry(false) = 15; // no advance, so next line overwrites.
    rtable.get_write_entry() = 16; // overwrite
    rtable.get_write_entry(false) = 17;
    rtable.done(); // advance, so next line does not overwrite.
    rtable.get_write_entry() = 18;

    Record_table_iterator<int> table_iter(rtable);
    table_iter.begin();
    EXPECT_EQ(14, table_iter.get_current());

    table_iter.next();
    EXPECT_EQ(16, table_iter.get_current());

    table_iter.next();
    EXPECT_EQ(17, table_iter.get_current());

    table_iter.next();
    EXPECT_EQ(18, table_iter.get_current());

    // After n written entries, reach the end.
    table_iter.next();
    EXPECT_TRUE(table_iter.end());
}

TEST_F(Record_table_test, rollover)
{
    Record_table<int> rtable(Record_table_config().size(2).enable());

    rtable.get_write_entry() = 13;
    rtable.get_write_entry() = 14;
    rtable.get_write_entry() = 15;

    Record_table_iterator<int> table_iter(rtable);

    table_iter.begin();
    EXPECT_FALSE(table_iter.end());
    EXPECT_EQ(14, table_iter.get_current());

    table_iter.next();
    EXPECT_FALSE(table_iter.end());
    EXPECT_EQ(15, table_iter.get_current());

    // After 2 entries (first of 3 was overwritten), reach the end.
    table_iter.next();
    EXPECT_TRUE(table_iter.end());
}

TEST_F(Record_table_test, oneshot)
{
    Record_table<int> rtable(Record_table_config().oneshot().size(2).enable());

    rtable.get_write_entry() = 13;
    rtable.get_write_entry() = 14;
    rtable.get_write_entry() = 15;
    EXPECT_FALSE(rtable.active());

    Record_table_iterator<int> table_iter(rtable);

    table_iter.begin();
    EXPECT_FALSE(table_iter.end());
    EXPECT_EQ(13, table_iter.get_current());

    table_iter.next();
    EXPECT_FALSE(table_iter.end());
    EXPECT_EQ(14, table_iter.get_current());

    // After 2 entries (last one was not saved because one-shot), reach the end.
    table_iter.next();
    EXPECT_TRUE(table_iter.end());
}

static constexpr unsigned NUM_STAMPS_PER_ENTRY = 2;

void dump_ts_array_cb(const std::array<timespec, NUM_STAMPS_PER_ENTRY> & entry)
{
    std::cout << "entry size " << entry.size() << std::endl;

    for (auto const & t : entry)
    {
        std::cout << t.tv_sec << "." << t.tv_nsec << std::endl;
    }
}

TEST_F(Record_table_test, timespec_record_array)
{
    using Double_stamp = std::array<timespec, NUM_STAMPS_PER_ENTRY>;
    constexpr unsigned NUM_ENTRIES = 3;
    Record_table<Double_stamp> rtable(Record_table_config().size(NUM_ENTRIES).enable(), dump_ts_array_cb);

    for (unsigned i = 0; i < NUM_ENTRIES * 2; ++i)
    {
        auto & w_entry = rtable.get_write_entry();
        for (unsigned j = 0; j < NUM_STAMPS_PER_ENTRY; ++j)
        {
            clock_gettime(CLOCK_REALTIME, &w_entry[j]);
        }
    }

    rtable.dump();
}

static void dump_int_cb(const int & entry)
{
    std::cout << "int entry " << entry << std::endl;
}

TEST_F(Record_table_test, mgr)
{
    Record_table<int> rtable(Record_table_config().size(12).enable(), dump_int_cb);

    rtable.get_write_entry() = 11001;

    Record_table_manager mgr({{"int-table", rtable}});
    mgr.dump_tables("int-t");
}

TEST_F(Record_table_test, mgr_match2)
{
    Record_table<int> rtable(Record_table_config().size(12).enable(), dump_int_cb);
    Record_table<int> rtable2(Record_table_config().size(12).enable(), dump_int_cb);
    Record_table<int> rtable3(Record_table_config().size(12).enable(), dump_int_cb);

    rtable.get_write_entry() = 11001;
    rtable2.get_write_entry() = 2202;
    rtable3.get_write_entry() = 333;

    Record_table_manager mgr({{"int-table1", rtable}, {"x", rtable2}, {"int-table3", rtable3}});
    mgr.dump_tables("int-t");
}

static void dump_table_state_cb(const Record_table_op_itf & t)
{
    std::cout << "enabled " << t.enabled() << std::endl;
    std::cout << "oneshot " << t.oneshot() << std::endl;
    std::cout << "size " << t.size() << std::endl;
    std::cout << "stopped " << t.is_stopped() << std::endl;
}

TEST_F(Record_table_test, mgr_state)
{
    Record_table<int> rtable(Record_table_config().size(12).enable(), nullptr, dump_table_state_cb);

    Record_table_manager mgr({{"int-table", rtable}});
    mgr.dump_tables_state("int-t");
}
