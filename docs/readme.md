This library is loosely coupled:
- As shown in the example, if the user adds more Record_tables to a Record_table manager in composition_root.cpp, the code that manages and displays the tables (in operator.cpp) does not have to change.
- The user can change the type of ENTRY in the tables, and neither the library code nor the code that manages and displays the tables (in operator.cpp) changes; the user just defines ENTRY and a function or class
that dumps the contents of the entry, e.g. prints it to stdout.

