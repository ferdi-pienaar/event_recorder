This library is loosely coupled to its clients:
- As shown in the example, if the user adds more Record_tables to a Record_table manager in composition_root.cpp, the code that manages and displays the tables (in operator.cpp) does not have to change.
- The user can change the type of ENTRY in the tables, and neither the event_recorder library code nor the code that manages and displays the tables (in operator.cpp) changes; the user just defines ENTRY and a function or class
that dumps the contents of the entry, e.g. prints it to stdout.
- The client can change the functions that dump entries without changing the library or changing the code that manages and displays the tables (in operator.cpp in the example).
- In the client code, only composition_root.cpp depends on the library implementation; operator.cpp and operator_helper.cpp depend only on an interface, record_table_manager_itf.h, and event_generator.cpp depends only on another interface, record_table_event_itf.h. This makes operator.cpp, operator_helper.cpp and event_generator.cpp testable independently of this library.

event_recorder can record events that happen before main() runs because the event_recorder's initialization dependencies can be satisfied before main() runs. This is shown in example_early.