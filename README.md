# Introduction
This library records events that occur at run-time in RAM tables. The saved tables of events can subsequently be displayed and managed via a manager controlled by e.g. a Command Line Interface.

The type of data recorded by an event is chosen by the user of this library, but a typical application is to save two timestamps: a timestamp marking the beginning of event-handling and another marking the end.

The management of the tables is decoupled from the types of event recorded in the tables: one manager handles many tables, each possibly recording a different type of event. Due to the decoupling between tables and the management interface, when a new table recording a new event type is added, the code that interfaces to the CLI for displaying and managing the tables does not even have to be re-compiled. Within the manager, each table is associated with a name, and by selecting the names the user can group the tables and manage them in groups.

For example, there are 4 tables for packet-handling events, named according to the protocols they handle and the packet direction: "rtcp-upstream", "rtcp-downstream", "dhcp-upstream", and "dhcp-downstream". The command "dump downstr" would dump the tables whose names contain this string, "rtcp-downstream" and "dhcp-downstream"; similarly "dump dhcp" would dump tables "dhcp-upstream" and "dhcp-downstream". Note that the manager has no knowledge of the domain, it selects tables by matching a string to the table names.

# Installation
CMake projects can install and use this library as follows:
Download the project and enter the project directory. Then configure, build and install the library:
```bash
cmake -S . -B build -DCMAKE_BUILD_TYPE=Release
cmake --build build
cmake --install build --prefix /path/to/somewhere
```

And to configure the project that uses this library:
```bash
cmake -S . -B build -DCMAKE_PREFIX_PATH=/path/to/somewhere
```
# Example code
## Directory 'example'
dependency_root.cpp defines two tables. One's entry type is two timestamps, corresponding to the beginning and end of an event. The other's entry type is an integer.

## Directory 'example_early'
This example shows how event_recorder records events that happen before main() runs because the event_recorder's initialization dependencies can be satisfied before main() runs.

# Design
This library is loosely coupled to its clients:
- As shown in the example, if the user adds more Record_tables to a Record_table manager in composition_root.cpp, the code that manages and displays the tables (in operator.cpp) does not have to change.
- The user can change the type of ENTRY in the tables, and neither the event_recorder library code nor the code that manages and displays the tables (in operator.cpp) changes; the user just defines ENTRY and a function or class
that dumps the contents of the entry, e.g. prints it to stdout.
- The client can change the functions that dump entries without changing the library or changing the code that manages and displays the tables (in operator.cpp in the example).
- In the client code, only composition_root.cpp depends on the library implementation; operator.cpp and operator_helper.cpp depend only on an interface, record_table_manager_itf.h, and event_generator.cpp depends only on another interface, record_table_event_itf.h. This makes operator.cpp, operator_helper.cpp and event_generator.cpp testable independently of this library.
