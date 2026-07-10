#ifndef _SIM_TIMEKEEPER_H_
#define _SIM_TIMEKEEPER_H_

#include "Smp/Services/ITimeKeeper.h"
#include "Smp/Services/IEventManager.h"
#include "Smp/Services/IScheduler.h"
#include "Smp/AnySimple.h"
#include "SimEntryPoint.hpp"

#include <chrono>
#include <memory>
#include <stdexcept>
#include <string>

namespace Azstsim::Kernel
{
    class TimeKeeper final : public Smp::Services::ITimeKeeper
    {
    public:
        static constexpr Smp::Char8 SMP_TimeKeeper[] = "TimeKeeper";

        explicit TimeKeeper(const std::string& name        = SMP_TimeKeeper,
                            const std::string& description = "",
                            Smp::IObject*      parent      = nullptr)
            : m_Name(name), m_Description(description), m_Parent(parent)
        {}

        ~TimeKeeper() noexcept override = default;

        /* ---- IObject ---- */
        Smp::String8  GetName()        const override { return m_Name.c_str();        }
        Smp::String8  GetDescription() const override { return m_Description.c_str(); }
        Smp::IObject* GetParent()      const override { return m_Parent;              }
        Smp::IObject* GetChild(Smp::String8) const override { return nullptr; }

        /* ---- IComponent ---- */
        Smp::ComponentStateKind GetState() const override { return m_State; }
        const Smp::Uuid& GetUuid() const override { return m_Uuid; }

        void Publish(Smp::IPublication*) override
        {
            if (m_State != Smp::ComponentStateKind::CSK_Created)
                throw std::runtime_error("Publish called in wrong state");
            m_State = Smp::ComponentStateKind::CSK_Publishing;
        }

        void Configure(Smp::Services::ILogger*, Smp::Services::ILinkRegistry* = nullptr) override
        {
            if (m_State != Smp::ComponentStateKind::CSK_Publishing)
                throw std::runtime_error("Configure called in wrong state");
            m_State = Smp::ComponentStateKind::CSK_Configured;
        }

        void Connect(Smp::ISimulator* simulator) override;

        void Disconnect() override
        {
            if (m_State != Smp::ComponentStateKind::CSK_Connected)
                throw std::runtime_error("Disconnect called in wrong state");
            m_EventMgr           = nullptr;
            m_Scheduler          = nullptr;
            m_InPreSimTimeChange = false;
            m_State = Smp::ComponentStateKind::CSK_Disconnected;
        }

        Smp::IField* GetField(Smp::String8 name) const override
        {
            throw std::invalid_argument(std::string("Field not found: ") + name);
        }
        const Smp::FieldCollection* GetFields() const override { return nullptr; }

        Smp::AnySimple GetSimpleValue(Smp::String8) const override { return {}; }
        void SetSimpleValue(Smp::String8, Smp::AnySimple) override {}
        void GetSimpleArrayValue(Smp::String8, Smp::UInt64, Smp::AnySimple*, Smp::UInt64) const override {}
        void SetSimpleArrayValue(Smp::String8, Smp::UInt64, Smp::AnySimpleArray, Smp::UInt64) override {}
        Smp::Bool AddChild(Smp::IObject*, const Smp::ICollectionBase*) override { return false; }
        Smp::Bool RemoveChild(Smp::IObject*, const Smp::ICollectionBase*) override { return false; }
        Smp::IObject* IsChildInCollection(Smp::String8, const Smp::ICollectionBase*) const override { return nullptr; }

        /* ---- ITimeKeeper ---- */
        Smp::Duration GetSimulationTime()   const override { return m_SimTime; }
        Smp::DateTime GetEpochTime()        const override { return m_EpochBase + m_SimTime; }
        Smp::DateTime GetMissionStartTime() const override { return m_MissionStart; }
        Smp::Duration GetMissionTime()      const override { return GetEpochTime() - m_MissionStart; }

        Smp::DateTime GetZuluTime() const override
        {
            using namespace std::chrono;
            return static_cast<Smp::DateTime>(
                duration_cast<seconds>(system_clock::now().time_since_epoch()).count());
        }

        void SetSimulationTime(Smp::Duration simulationTime) override;
        void SetEpochTime(Smp::DateTime epochTime) override;
        void SetMissionStartTime(Smp::DateTime missionStart) override;
        void SetMissionTime(Smp::Duration missionTime) override;

    private:
        /* IObject state */
        std::string   m_Name;
        std::string   m_Description;
        Smp::IObject* m_Parent{nullptr};

        /* IComponent state */
        Smp::ComponentStateKind m_State{Smp::ComponentStateKind::CSK_Created};
        Smp::Uuid               m_Uuid{makeUuid_()};

        /* ITimeKeeper state */
        Smp::Duration m_SimTime{0};
        Smp::DateTime m_EpochBase{0};
        Smp::DateTime m_MissionStart{0};

        /* Wiring */
        Smp::Services::IEventManager* m_EventMgr{nullptr};
        Smp::Services::IScheduler*    m_Scheduler{nullptr};

        /* SetSimulationTime gating (see 5.3.2e) */
        bool m_InPreSimTimeChange{false};

        std::unique_ptr<SimpleEntryPoint> m_PreSimEp;
        std::unique_ptr<SimpleEntryPoint> m_PostSimEp;

        void onPreSimTimeChange_()  noexcept { m_InPreSimTimeChange = true;  }
        void onPostSimTimeChange_() noexcept { m_InPreSimTimeChange = false; }

        static Smp::Uuid makeUuid_()
        {
            static int counter = 0;
            return Smp::Uuid(static_cast<uint32_t>(counter++), {0,0,0}, {0,0,0,0,0,0});
        }
    };
} // namespace Azstsim::Kernel

#endif // _SIM_TIMEKEEPER_H_
