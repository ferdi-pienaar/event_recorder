
struct timespec;
namespace Event_record
{
template <typename ENTRY> class Table_event_itf;
}

class Event_generator
{
public:
    Event_generator(Event_record::Table_event_itf<timespec> &);
    int run();

private:
    Event_record::Table_event_itf<timespec> &m_table;
};
