
struct timespec;
template <typename ENTRY> class Record_table_event_itf;

class Event_generator
{
public:
    Event_generator(Record_table_event_itf<timespec> &);
    int run();

private:
    Record_table_event_itf<timespec> & m_table;
};
