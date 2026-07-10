#ifndef SMP_SIMPLE_ENTRYPOINT_H_
#define SMP_SIMPLE_ENTRYPOINT_H_

#include "Smp/IEntryPoint.h"
#include <functional>
#include <string>

    class SimpleEntryPoint final : public Smp::IEntryPoint
    {
    public:
        using Fn = std::function<void()>;

        SimpleEntryPoint(std::string name,
                         std::string description,
                         Smp::IObject* parent,
                         Fn fn)
            : name_(std::move(name))
            , desc_(std::move(description))
            , parent_(parent)
            , fn_(std::move(fn))
        {}

        ~SimpleEntryPoint() noexcept override = default;

        // IEntryPoint
        void Execute() const override { if (fn_) fn_(); }

        // IObject
        Smp::String8 GetName() const override { return name_.c_str(); }
        Smp::String8 GetDescription() const override { return desc_.c_str(); }
        Smp::IObject* GetParent() const override { return parent_; }
        Smp::IObject *GetChild(Smp::String8 name) const override { return nullptr; }

      private:
        std::string name_;
        std::string desc_;
        Smp::IObject* parent_{nullptr};
        Fn fn_;
    };

#endif // SMP_SIMPLE_ENTRYPOINT_H_
