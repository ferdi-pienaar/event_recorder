/*
 */
#include <iostream>
#include "record_table.h"
#include "gtest/gtest.h"
#include "time.h"
#include <gperftools/profiler.h>

class Record_table_perf_test : public testing::Test
{
  protected:
    Record_table_perf_test() : table(Record_table_init_config().size(10).enable())
    {
        itable = &table;
        // NB: You should verify that optimizer isn't removing virtual function call
        // e.g. by using gdb to check if there is a direct function call in the test.
        itable_itf = &table;
    }

    virtual void SetUp()
    {
        clock_gettime(CLOCK_REALTIME, &t1);
    }

    virtual void TearDown()
    {
        timespec t2;
        clock_gettime(CLOCK_REALTIME, &t2);

        double elapsed = t2.tv_sec - t1.tv_sec + 1e-9 * (t2.tv_nsec - t1.tv_nsec);

        std::cout << "\nElapsed time: " << elapsed <<  "s" << std::endl;
    }

    Record_table<int> table;
    Record_table<int> * itable = nullptr;
    Record_table_event_itf<int> * itable_itf = nullptr;
    timespec t1;
    static constexpr unsigned LOOP_MAX = 1 << 30;
};

TEST_F(Record_table_perf_test, use_direct)
{
    ProfilerStart("chrono_d.prof");
    for (int i = 0; i < LOOP_MAX; ++i)
    {
        itable->get_write_entry() = i;
    }
    ProfilerStop();
}

TEST_F(Record_table_perf_test, use_interface)
{
    ProfilerStart("chrono_i.prof");
    for (int i = 0; i < LOOP_MAX; ++i)
    {
        itable_itf->get_write_entry() = i;
    }
    ProfilerStop();
}
