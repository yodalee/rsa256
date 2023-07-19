#pragma once
#include <array>
#include <vector>

namespace verilog {

enum class ModuleEventId : unsigned {
	kReset = 0,
	kClockComb0,
	kClockComb1,
	kClockSeq0,
	kClockSeq1,
	kNumModuleEventSlot
};
class ModuleBase;

struct ModuleEventListener {
	ModuleBase* child;
	ModuleEventId module_event_id;
};

enum class EventConnectionStatus : unsigned {
	kSlotReady,
	kSlotConnected,
	kSlotNotExist
};

class ModuleBase {
protected:
	struct EventSlot {
		// When this event slot is triggered, we will also trigger these sub-events
		std::vector<ModuleEventListener> children_slots;
		// Bookkeeping, will error-check when someone connects to a non-existing slot
		EventConnectionStatus slot_status;
	};
	std::array<
		EventSlot,
		unsigned(ModuleEventId::kNumModuleEventSlot)
	> event_slots_;
	void Reset() {}
	void ClockComb0() {}
	void ClockComb1() {}
	void ClockSeq0() {}
	void ClockSeq1() {}
	// If the function of that event slot is the base class function, then we mark the slot "not existed"
	// Later we can marked it is connected to a submodule slot (see Connect())
	template<typename T>
	void BookkeepSlotExist() {
		auto f = [](bool eq) { return eq ? EventConnectionStatus::kSlotNotExist : EventConnectionStatus::kSlotReady; };
		event_slots_[unsigned(ModuleEventId::kReset)].slot_status = f(&T::Reset == &ModuleBase::Reset);
		event_slots_[unsigned(ModuleEventId::kClockComb0)].slot_status = f(&T::ClockComb0 == &ModuleBase::ClockComb0);
		event_slots_[unsigned(ModuleEventId::kClockComb1)].slot_status = f(&T::ClockComb1 == &ModuleBase::ClockComb1);
		event_slots_[unsigned(ModuleEventId::kClockSeq0)].slot_status = f(&T::ClockSeq0 == &ModuleBase::ClockSeq0);
		event_slots_[unsigned(ModuleEventId::kClockSeq1)].slot_status = f(&T::ClockSeq1 == &ModuleBase::ClockSeq1);
	}

	// Connect module clock to submodule's clock
	// Shall only be called in a module
	void Connect(
		ModuleBase& submodule,
		const ModuleEventId cur_module_event_id,
		const ModuleEventId submodule_event_id
	) {
		auto& slot = event_slots_[unsigned(cur_module_event_id)];
		slot.slot_status = EventConnectionStatus::kSlotReady;
		auto& child = slot.children_slots;
		child.push_back({&submodule, submodule_event_id});
	}
public:
	EventConnectionStatus GetConnectionStatus(ModuleEventId i) {
		return event_slots_[unsigned(i)].slot_status;
	}

	virtual void NotifySlot(ModuleEventId event_id) = 0;
	virtual ~ModuleBase() {}
};

// CRTP class Module, T is the user-defined module
template<typename T>
class Module : public ModuleBase {
public:
	Module() {
		BookkeepSlotExist<T>();
	}
	void NotifySlot(ModuleEventId event_id) override {
		T& self = dynamic_cast<T&>(*this);
		switch (event_id) {
			case ModuleEventId::kReset: { self.Reset(); break; }
			case ModuleEventId::kClockComb0: { self.ClockComb0(); break; }
			case ModuleEventId::kClockComb1: { self.ClockComb1(); break; }
			case ModuleEventId::kClockSeq0: { self.ClockSeq0(); break; }
			case ModuleEventId::kClockSeq1: { self.ClockSeq1(); break; }
			default: { break; }
		}
		auto& slot = event_slots_[unsigned(event_id)];
		for (auto& child : slot.children_slots) {
			child.child->NotifySlot(child.module_event_id);
		}
	}
};

} // namespace verilog
