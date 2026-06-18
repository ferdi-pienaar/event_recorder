/*
 *
 */
#pragma once
#include "example_event_data_types.h"

template <typename ENTRY> class Record_table_event_itf;

int event_generator_init(Record_table_event_itf<Double_stamp> &tt, Record_table_event_itf<int> &it);
