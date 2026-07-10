#include <map>
#include <string>
#include <iterator>
#include <iostream>

struct Event
{
    int eventKind;
    int id;
};

struct MapStruct
{
    //std::multimap<int, Event>::iterator it;
    std::multimap<int, Event> SimEvents;
    std::multimap<int, Event> MissEvents;
};

int main()
{
    MapStruct mapS;

    mapS.SimEvents.emplace(1, (11,22));
    mapS.SimEvents.emplace(1, (11,22));
    mapS.MissEvents.emplace(1, (11,22));
    mapS.MissEvents.emplace(1, (11,22));

    std::multimap<int, Event>::iterator it = mapS.SimEvents.begin();

    for


}
