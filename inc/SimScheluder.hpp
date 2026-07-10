#ifndef _SIM_SCHEDULER_H_
#define _SIM_SCHEDULER_H_

#define MAX_INT INT64_MAX

#include <map>
#include <list>

#include <functional>
#include "Smp/Int64.h"
#include "Smp/Duration.h"
#include "Smp/DateTime.h"
#include "SimObject.hpp"
#include "SimService.hpp"
#include "SimScheluder.hpp"
#include "SimTimeKeeper.hpp"
#include "Smp/IEntryPoint.h"
#include "Smp/Services/EventId.h"
#include "Smp/Services/IScheduler.h"

#include "SimInvalidEventId.hpp"
#include "SimInvalidCycleTime.hpp"
#include "SimInvalidEventTime.hpp"
#

namespace Scheduler
{
    class SimEvent
    {
        public: 

        SimEvent(const Smp::IEntryPoint* EntryPoint, Smp::Duration cycleTime, Smp::Int64 repeat, Smp::Services::EventId Id)
        : repeat_(repeat), cycleTime_(cycleTime), Id_(Id), EntryPoint_(EntryPoint) {

        }

        Smp::Int64 repeat_;
        Smp::Duration cycleTime_;
        Smp::Services::EventId Id_;
        const Smp::IEntryPoint* EntryPoint_;
    }; // Class: SimEvent

    class SimScheduler: public SimService, public Smp::Services::IScheduler
    {
        public:

        SimScheduler(Smp::String8 ObjectName, Azstsim::Kernel::TimeKeeper* TimeKeeper);

        void Step(Smp::Duration simulationTime);

        /// Add an immediate event to the scheduler.
        /// An immediate event is an event that will be added as a
        /// simulation time event (with simulation delta time of 0) at the
        /// end of the list of immediate events; the list of immediate
        /// events is processed in order of insertion and before any
        /// non-immediate event. After adding the new Event to the
        /// scheduler, it returns the EventId identifying the added Event
        /// As an immediate event, it will be executed when the scheduler
        /// processes its simulation time events again, but not immediately
        /// in the call to AddImmediateEvent(). When the simulator is in
        /// Standby state, simulation time does not progress, and
        /// simulation time events (including immediate events) are not
        /// processed.
        /// @remarks To execute an entry point immediately without going
        /// through the scheduler, its Execute() method can be called.
        /// @remarks For events scheduled at the same time (including
        /// immediate events), the Scheduler policy is to always execute
        /// those first that have been posted first ("First scheduled,
        /// first executed").
        /// @param   entryPoint Entry point to call from event.
        /// @return  Event identifier that can be used to change or remove
        /// event.
        Smp::Services::EventId AddImmediateEvent(
            const Smp::IEntryPoint* entryPoint) override;

        /// Add event to scheduler that is called based on simulation time.
        /// If the provided Simulation Time is more than MAX_INT – ‘current
        /// simulation time’ , it throws an InvalidEventTime exception, the
        /// Event is not added to the scheduler and never executed. If the
        /// provided Simulation Time is -1, the event is created but not
        /// posted (i.e. not on the timeline). It will be posted only when
        /// SetEventSimulationTime is used to define a valid execution
        /// time. If the provided Simulation Time is a negative value less
        /// than -1, it throws an InvalidEventTime exception, the Event is
        /// not added to the scheduler and never executed. If Repeat is not
        /// zero and CycleTime is a value less than -1 or is 0, it throws
        /// an InvalidCycleTime exception, the Event is not added to the
        /// scheduler and never executed; If the provided Simulation Time
        /// is -1, the event is created but not posted on the timeline; If
        /// after the event execution the Cycle Time is -1 and repeat count
        /// is not 0, then the event is not re-posted on the timeline ; An
        /// event not posted on the timeline will be posted only until
        /// SetEventSimulationTime is used to define a valid execution
        /// time; After adding the new Event to the scheduler, it returns
        /// the EventId identifying the added Event.
        /// @remarks For events scheduled at the same time (including
        /// immediate events), the Scheduler policy is to always execute
        /// those first that have been posted first ("First scheduled,
        /// first executed").
        /// @param   entryPoint Entry point to call from event.
        /// @param   simulationTime Duration from now when to trigger the
        /// event for the first time.
        /// This must not be a negative simulation time, as this would be
        /// in the past.
        /// @param   cycleTime Duration between two triggers of the event.
        /// For non-cyclic Events, the cycle time parameter is stored, but
        /// not used;
        /// For cyclic Events, an InvalidCycleTime exception is thrown if
        /// the cycle time is is a value less than -1 or is 0.
        /// @param   repeat Number of times the event shall be repeated, or
        /// 0 for a single event, or -1 for no limit.
        /// An Event with repeat=0 is non-cyclic and executes only once; 
        /// An Event with repeat=0 is removed automatically after its
        /// triggering; 
        /// An Event with repeat>0 is cyclic, and repeats ‘repeat’ times;
        /// An Event with repeat>0 is removed automatically after it has
        /// been triggered ‘repeat+1’ times;
        /// An Event with repeat<0 is cyclic forever;
        /// An Event with repeat<0 is never removed from the scheduler
        /// unless explicitly requested using the RemoveEvent() method.
        /// @return  Event identifier that can be used to change or remove
        /// event.
        /// @throws  Smp::Services::InvalidCycleTime
        /// @throws  Smp::Services::InvalidEventTime
        Smp::Services::EventId AddSimulationTimeEvent(
            const Smp::IEntryPoint* entryPoint,
            Smp::Duration simulationTime,
            Smp::Duration cycleTime = 0,
            Smp::Int64 repeat = 0) override;

        /// Add event to scheduler that is called based on mission time.
        /// If the Mission Time is less than the current mission time, it
        /// throws an InvalidEventTime exception, the Event is not added to
        /// the scheduler and never executed; If Repeat is not zero and
        /// CycleTime is a value less than 1, it throws an InvalidCycleTime
        /// exception, the Event is not added to the scheduler and never
        /// executed; After adding the new Event to the scheduler, it
        /// returns the EventId identifying the added Event.
        /// @remarks For events scheduled at the same time (including
        /// immediate events), the Scheduler policy is to always execute
        /// those first that have been posted first ("First scheduled,
        /// first executed").
        /// @param   entryPoint Entry point to call from event.
        /// @param   missionTime Absolute mission time when to trigger the
        /// event for the first time.
        /// This must not be a mission time in the past.
        /// @param   cycleTime Duration between two triggers of the event.
        /// For non-cyclic Events, the cycle time parameter is stored, but
        /// not used;
        /// For cyclic Events, an InvalidCycleTime exception is thrown if
        /// the cycle time is a value less than 1.
        /// @param   repeat Number of times the event shall be repeated, or
        /// 0 for a single event, or -1 for no limit.
        /// An Event with repeat=0 is non-cyclic and executes only once; 
        /// An Event with repeat=0 is removed automatically after its
        /// triggering; 
        /// An Event with repeat>0 is cyclic, and repeats ‘repeat’ times;
        /// An Event with repeat>0 is removed automatically after it has
        /// been triggered ‘repeat+1’ times;
        /// An Event with repeat<0 is cyclic forever;
        /// An Event with repeat<0 is never removed from the scheduler
        /// unless explicitly requested using the RemoveEvent() method.
        /// @return  Event identifier that can be used to change or remove
        /// event.
        /// @throws  Smp::Services::InvalidCycleTime
        /// @throws  Smp::Services::InvalidEventTime
        Smp::Services::EventId AddMissionTimeEvent(
            const Smp::IEntryPoint* entryPoint,
            Smp::Duration missionTime,
            Smp::Duration cycleTime = 0,
            Smp::Int64 repeat = 0) override;

        /// Add event to scheduler that is called based on epoch time.
        /// If the Epoch Time is less than the current epoch time it throws
        /// an InvalidEventTime exception, the Event is not added to the
        /// scheduler and never executed; If Repeat is not zero and
        /// CycleTime is a value less than 1, it throws an InvalidCycleTime
        /// exception, the Event is not added to the scheduler and never
        /// executed; After adding the new Event to the scheduler, it
        /// returns the EventId identifying the added Event.
        /// @remarks For events scheduled at the same time (including
        /// immediate events), the Scheduler policy is to always execute
        /// those first that have been posted first ("First scheduled,
        /// first executed").
        /// @param   entryPoint Entry point to call from event.
        /// @param   epochTime Epoch time when to trigger the event for the
        /// first time.
        /// This must not be an epoch time in the past.
        /// @param   cycleTime Duration between two triggers of the event.
        /// For non-cyclic Events, the cycle time parameter is stored, but
        /// not used;
        /// For cyclic Events, an InvalidCycleTime exception is thrown if
        /// the cycle time is a value less than 1.
        /// @param   repeat Number of times the event shall be repeated, or
        /// 0 for a single event, or -1 for no limit.
        /// An Event with repeat=0 is non-cyclic and executes only once; 
        /// An Event with repeat=0 is removed automatically after its
        /// triggering; 
        /// An Event with repeat>0 is cyclic, and repeats ‘repeat’ times;
        /// An Event with repeat>0 is removed automatically after it has
        /// been triggered ‘repeat+1’ times;
        /// An Event with repeat<0 is cyclic forever;
        /// An Event with repeat<0 is never removed from the scheduler
        /// unless explicitly requested using the RemoveEvent() method.
        /// @return  Event identifier that can be used to change or remove
        /// event.
        /// @throws  Smp::Services::InvalidCycleTime
        /// @throws  Smp::Services::InvalidEventTime
        Smp::Services::EventId AddEpochTimeEvent(
            const Smp::IEntryPoint* entryPoint,
            Smp::DateTime epochTime,
            Smp::Duration cycleTime = 0,
            Smp::Int64 repeat = 0) override;

        /// Add event to scheduler that is called based on Zulu time.
        /// If the given Zulu Time is less than the current Zulu time, it
        /// throws an InvalidEventTime exception, the Event is not added to
        /// the scheduler and never executed; If Repeat is not zero and
        /// CycleTime is a value less than 1, it throws an InvalidCycleTime
        /// exception, the Event is not added to the scheduler and never
        /// executed; After adding the new Event to the scheduler, it
        /// returns the EventId identifying the added Event.
        /// @remarks For events scheduled at the same time (including
        /// immediate events), the Scheduler policy is to always execute
        /// those first that have been posted first ("First scheduled,
        /// first executed").
        /// @param   entryPoint Entry point to call from event.
        /// @param   zuluTime Absolute (Zulu) time when to trigger the
        /// event for the first time.
        /// This must not be a time in the past.
        /// @param   cycleTime Duration between two triggers of the event.
        /// For non-cyclic Events, the cycle time parameter is stored, but
        /// not used;
        /// For cyclic Events, an InvalidCycleTime exception is thrown if
        /// the cycle time is a value less than 1.
        /// @param   repeat Number of times the event shall be repeated, or
        /// 0 for a single event, or -1 for no limit.
        /// An Event with repeat=0 is non-cyclic and executes only once; 
        /// An Event with repeat=0 is removed automatically after its
        /// triggering; 
        /// An Event with repeat>0 is cyclic, and repeats ‘repeat’ times;
        /// An Event with repeat>0 is removed automatically after it has
        /// been triggered ‘repeat+1’ times;
        /// An Event with repeat<0 is cyclic forever;
        /// An Event with repeat<0 is never removed from the scheduler
        /// unless explicitly requested using the RemoveEvent() method.
        /// @return  Event identifier that can be used to change or remove
        /// event.
        /// @throws  Smp::Services::InvalidCycleTime
        /// @throws  Smp::Services::InvalidEventTime
        Smp::Services::EventId AddZuluTimeEvent(
            const Smp::IEntryPoint* entryPoint,
            Smp::DateTime zuluTime,
            Smp::Duration cycleTime = 0,
            Smp::Int64 repeat = 0) override;

        /// Add event to scheduler that is called based on Zulu time.
        /// An event with repeat=0 is not cyclic. It will be removed
        /// automatically after is has been triggered. An event with
        /// repeat>0 is cyclic, and will be repeated repeat times.
        /// Therefore, it will be called repeat+1 times, and then it will
        /// be removed automatically. An event with repeat=-1 is cyclic as
        /// well, but it will be triggered forever, unless it is removed
        /// from the scheduler using the RemoveEvent() method. For a cyclic
        /// event, the cycleTime needs to be positive. Otherwise, an
        /// InvalidCycleTime exception is thrown. For non-cyclic events, it
        /// is stored but not used. The zuluTimedelay must not be negative.
        /// Otherwise, an InvalidEventTime exception is thrown.
        /// @remarks For events scheduled at the same time (including
        /// immediate events), the Scheduler policy is to always execute
        /// those first that have been posted first ("First scheduled,
        /// first executed").
        /// @param   entryPoint Entry point to call from event.
        /// @param   zuluTimeDelay Relative (Zulu) time when to trigger the
        /// event for the first time.
        /// This must not be a negative duration.
        /// @param   cycleTime Duration between two triggers of the event.
        /// When repeat is not zero, this must be a positive duration.
        /// @param   repeat Number of times the event shall be repeated, or
        /// 0 for a single event, or -1 for no limit.
        /// @return  Event identifier that can be used to change or remove
        /// event.
        /// @throws  Smp::Services::InvalidCycleTime
        /// @throws  Smp::Services::InvalidEventTime
        Smp::Services::EventId AddRelativeZuluTimeEvent(
            const Smp::IEntryPoint* entryPoint,
            Smp::Duration zuluTimeDelay,
            Smp::Duration cycleTime = 0,
            Smp::Int64 repeat = 0) override;

        /// Update when an existing simulation time event on the scheduler
        /// shall be triggered.
        /// The simulationTime must not be negative. Otherwise, the event
        /// will never be executed, but immediately removed.
        /// When the given event Id is not a valid identifier of a
        /// scheduler event, the method throws an exception of type
        /// InvalidEventId. In case an event is registered under the given
        /// event Id but it is not a simulation time event, the method
        /// throws an exception of type InvalidEventId as well.
        /// @param   event Identifier of event to modify.
        /// @param   simulationTime Duration from now when to trigger event.
        /// If the simulation time is negative, the event will never be
        /// executed but instead be removed immediately from the scheduler.
        /// @throws  Smp::Services::InvalidEventId
        void SetEventSimulationTime(
            Smp::Services::EventId event,
            Smp::Duration simulationTime) override;

        /// Update when an existing mission time event on the scheduler
        /// shall be triggered.
        /// The missionTime must not be before the current mission time of
        /// the ITimeKeeper service. Otherwise, the event will never be
        /// executed, but immediately removed.
        /// When the given event Id is not a valid identifier of a
        /// scheduler event, the method throws an exception of type
        /// InvalidEventId. In case an event is registered under the given
        /// event Id but it is not a mission time event, the method throws
        /// an exception of type InvalidEventId as well.
        /// @param   event Identifier of event to modify.
        /// @param   missionTime Absolute mission time when to trigger
        /// event.
        /// If the mission time is before the current mission time, the
        /// event will never be executed but instead be removed immediately
        /// from the scheduler.
        /// @throws  Smp::Services::InvalidEventId
        void SetEventMissionTime(
            Smp::Services::EventId event,
            Smp::Duration missionTime) override;

        /// Update when an existing epoch time event on the scheduler (an
        /// event that has been registered using AddEpochTimeEvent()) shall
        /// be triggered.
        /// The epochTime must not be before the current epoch time of the
        /// ITimeKeeper service. Otherwise, the event will never be
        /// executed, but immediately removed.
        /// When the given event Id is not a valid identifier of a
        /// scheduler event, the method throws an exception of type
        /// InvalidEventId. In case an event is registered under the given
        /// event Id but it is not an epoch time event, the method throws
        /// an exception of type InvalidEventId as well.
        /// @param   event Identifier of event to modify.
        /// @param   epochTime Epoch time when to trigger event.
        /// If the epoch time is before the current epoch time, the event
        /// will never be executed but instead be removed immediately from
        /// the scheduler.
        /// @throws  Smp::Services::InvalidEventId
        void SetEventEpochTime(
            Smp::Services::EventId event,
            Smp::DateTime epochTime) override;

        /// Update when an existing zulu time event on the scheduler shall
        /// be triggered.
        /// The zuluTime must not be before the current Zulu time of the
        /// ITimeKeeper service. Otherwise, the event will never be
        /// executed, but immediately removed.
        /// When the given event Id is not a valid identifier of a
        /// scheduler event, the method throws an exception of type
        /// InvalidEventId. In case an event is registered under the given
        /// event Id but it is not a zulu time event, the method throws an
        /// exception of type InvalidEventId as well.
        /// @param   event Identifier of event to modify.
        /// @param   zuluTime Absolute (Zulu) time when to trigger event.
        /// If the zulu time is before the current zulu time, the event
        /// will never be executed but instead be removed immediately from
        /// the scheduler.
        /// @throws  Smp::Services::InvalidEventId
        void SetEventZuluTime(
            Smp::Services::EventId event,
            Smp::DateTime zuluTime) override;

        /// Update cycle time of an existing event on the scheduler.
        /// For a cyclic event, the cycleTime needs to be positive. For
        /// non-cyclic events, it is stored but not used.
        /// When the given event is not a valid identifier of a scheduler
        /// event, the method throws an exception of type InvalidEventId.
        /// @param   event Identifier of event to modify.
        /// @param   cycleTime Duration between two triggers of the event.
        /// For a cyclic event, this needs to be a positive duration.
        /// Otherwise, an exception of type InvalidCycleTime is thrown.
        /// @throws  Smp::Services::InvalidEventId
        /// @throws  Smp::Services::InvalidCycleTime
        void SetEventCycleTime(
            Smp::Services::EventId event,
            Smp::Duration cycleTime) override;

        /// Update the repeat counter of an existing event on the scheduler.
        /// An event with repeat=0 is not cyclic. It will be removed
        /// automatically after is has been triggered. An event with
        /// repeat>0 is cyclic, and will be repeated 'repeat' times.
        /// Therefore, it will be called repeat+1 times, and then it will
        /// be removed automatically. An event with repeat=-1 is cyclic as
        /// well, but it will be triggered forever, unless it is removed
        /// from the scheduler using the RemoveEvent() method. For a cyclic
        /// event, the cycleTime needs to be positive. For non-cyclic
        /// events, it is ignored.
        /// When the given event is not a valid identifier of a scheduler
        /// event, the method throws an exception of type InvalidEventId.
        /// @param   event Identifier of event to modify.
        /// @param   repeat Number of times the event shall be repeated, or
        /// 0 for a single event, or -1 for no limit.
        /// An event with a repeat different from 0 is called cyclic. For
        /// such an event, a positive cycle time has to be defined before.
        /// Otherwise, an exception of type InvalidCycleTime is thrown.
        /// @throws  Smp::Services::InvalidEventId
        void SetEventRepeat(
            Smp::Services::EventId event,
            Smp::Int64 repeat) override;

        /// Remove an event from the scheduler.
        /// An event with repeat=0 is removed automatically after it has
        /// been triggered.
        /// When the given event is not a valid identifier of a scheduler
        /// event, the method throws an exception of type InvalidEventId.
        /// @param   event Event identifier of the event to remove.
        /// @throws  Smp::Services::InvalidEventId
        void RemoveEvent(
            Smp::Services::EventId event) override;

        /// Return the ID of the event currently executed by the scheduler.
        /// If not event is executed, this function returns -1.
        /// The operation returns -1 when no event is being executed.
        /// @return  Event Id or -1 if no event is being executed.
        Smp::Services::EventId GetCurrentEventId() const override;

        /// Get the time of next scheduled Event.
        /// This function can only provide the time of the next event at
        /// the time of invocation. There is no guarantee that no events
        /// will be inserted between the method invocation and the
        /// (currently) next event. Events scheduled in Zulu Time are not
        /// considered, as these Events do not have a fixed defined
        /// Simulation Time.
        /// @return  Time of the next event on the scheduler.
        Smp::Duration GetNextScheduledEventTime() const override;

        /// Returns true if an event with the given eventId is scheduled,
        /// false otherwise.
        /// @param   eventId Event identifier to check for.
        /// @return  True if the event with the given eventId is scheduled
        /// (i.e. will be called again), false otherwise.
        Smp::Bool IsEventScheduled(
            Smp::Services::EventId eventId) const override;

        private:

        std::multimap<Smp::Duration, Scheduler::SimEvent> SimulationTimeEventQueue_;
        std::multimap<Smp::DateTime, Scheduler::SimEvent> ZuluTimeEventQueue_;
        std::list<Scheduler::SimEvent> ImmediateEventQueue_;
        std::list<Scheduler::SimEvent> PendingEventQueue_;
        Scheduler::SimEvent* currentExecutingEvent_{nullptr};
        Azstsim::Kernel::TimeKeeper* TimeKeeper_{nullptr}; 
    }; // Class: SimScheduler
}; // Namespace: Scheduler

#endif // _SIM_SCHEDULER_H_