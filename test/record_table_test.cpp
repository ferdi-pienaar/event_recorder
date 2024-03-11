/*
 *
 */

//#include <iostream>
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
    EXPECT_EQ(0, c.m_size);
    EXPECT_FALSE(c.m_enabled);
}

// Can't enable unless size > 0.
TEST_F(Record_table_test, config_enable_no_entries)
{
    Record_table_config c = Record_table_config().enable();
    EXPECT_EQ(0, c.m_size);
    EXPECT_FALSE(c.m_enabled);
}

TEST_F(Record_table_test, config_size_enable)
{
    Record_table_config c = Record_table_config().size(14).enable();
    EXPECT_EQ(14, c.m_size);
    EXPECT_TRUE(c.m_enabled);
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

    EXPECT_TRUE(rtable.size(6));
    EXPECT_TRUE(rtable.enable(true));
    EXPECT_TRUE(rtable.enabled());
}

TEST_F(Record_table_test, iterator_first)
{
    Record_table<int> rtable(Record_table_config().size(12).enable());

    rtable.write_entry() = 13;

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
    rtable.write_entry() = 13;

    Record_table_iterator<int> table_iter(rtable);
    table_iter.begin();
    EXPECT_TRUE(table_iter.end());
}

TEST_F(Record_table_test, table_disabled_write)
{
    Record_table<int> rtable(Record_table_config().size(12));
    rtable.write_entry() = 13;

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
    rtable.write_entry() = 13;

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
    rtable.write_entry() = 13;
    rtable.write_entry() = 14;

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

TEST_F(Record_table_test, rollover)
{
    Record_table<int> rtable(Record_table_config().size(2).enable());

    rtable.write_entry() = 13;
    rtable.write_entry() = 14;
    rtable.write_entry() = 15;

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

    rtable.write_entry() = 13;
    rtable.write_entry() = 14;
    rtable.write_entry() = 15;
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

void dummy_cb(const std::array<timespec, 2> & entry)
{
    for (auto const & t : entry)
    {
        std::cout << "entry size " << entry.size() << " " << t.tv_sec << "." << t.tv_nsec << std::endl;
    }
}

TEST_F(Record_table_test, timespec_record_2)
{
    using Double_stamp = std::array<timespec, 2>;
    constexpr unsigned NUM_ENTRIES = 5;
    Record_table<Double_stamp> rtable(Record_table_config().size(NUM_ENTRIES).enable(), dummy_cb);

    for (unsigned i = 0; i < NUM_ENTRIES * 2; ++i)
    {
        auto & w_entry = rtable.write_entry();
        clock_gettime(CLOCK_REALTIME, &w_entry[0]);
        clock_gettime(CLOCK_REALTIME, &w_entry[1]);
    }

    rtable.do_callback();
}

void dummy_int_cb(const int & entry)
{
    std::cout << "int entry " << entry << std::endl;
}

TEST_F(Record_table_test, mgr)
{
    Record_table<int> rtable(Record_table_config().size(12).enable(), dummy_int_cb);

    rtable.write_entry() = 11001;

    Record_table_manager mgr({{"int-table", rtable}});
    mgr.callback_entries("int-table");
}
