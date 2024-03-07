/*
 *
 */

//#include <iostream>
#include "record_table.h"
#include "record_table_iterator.h"
#include "record_table_manager.h"
#include "gtest/gtest.h"


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

TEST_F(Record_table_test, advance)
{
    Record_table<int> rtable(Record_table_config().size(12).enable());

    auto & entry = rtable.advance().write_entry();
    entry = 11;
}

TEST_F(Record_table_test, iterator_first)
{
    Record_table<int> rtable(Record_table_config().size(12).enable());

    rtable.advance().write_entry() = 13;

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
    rtable.advance().write_entry() = 13;

    Record_table_iterator<int> table_iter(rtable);
    table_iter.begin();
    EXPECT_TRUE(table_iter.end());
}

TEST_F(Record_table_test, table_disabled_write)
{
    Record_table<int> rtable(Record_table_config().size(12));
    rtable.advance().write_entry() = 13;

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
    rtable.advance().write_entry() = 13;

    Record_table_iterator<int> table_iter(rtable);
    table_iter.begin();
    EXPECT_FALSE(table_iter.end());

    rtable.clear();
    // Verify no entries after clear.
    table_iter.begin();
    EXPECT_TRUE(table_iter.end());
}

TEST_F(Record_table_test, iterator_next)
{
    Record_table<int> rtable(Record_table_config().size(12).enable());
    rtable.advance().write_entry() = 13;
    rtable.advance().write_entry() = 14;

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

    rtable.advance().write_entry() = 13;
    rtable.advance().write_entry() = 14;
    rtable.advance().write_entry() = 15;

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

TEST_F(Record_table_test, mgr)
{
    Record_table<int> rtable(Record_table_config().size(12).enable());

    Record_table_manager mgr({{"int-table", rtable}});
}
