/*
 *
 */
#pragma once
#include "example_event_data_types.h"

namespace Event_record
{
template <typename ENTRY> class Table_event_itf;
}

int event_generator_init(Event_record::Table_event_itf<Double_stamp> &,
                         Event_record::Table_event_itf<int> &,
                         Event_record::Table_event_itf<int> &);
