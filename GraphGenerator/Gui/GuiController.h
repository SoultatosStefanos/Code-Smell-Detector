#pragma once

#include <cassert>
#include <string_view>

namespace gui {

	class Gui;

	class GuiController final {
	public:
		GuiController(const GuiController&) = delete;
		GuiController(GuiController&&) = delete;

		GuiController& operator=(const GuiController&) = delete;	
		GuiController& operator=(GuiController&&) = delete;		

		static GuiController& GetSingleton();

		unsigned GetTotalUnits(void) const { return m_total_units; }
		void SetTotalUnits(unsigned units) { m_total_units = units; }

		unsigned GetUnitsDone(void) const { return m_units_done; }

		const Gui& GetGui(void) const;
		Gui& GetGui(void);

		bool IsGuiProgressDone(void) const { return GetTotalUnits() == GetUnitsDone(); }
		void AdvanceGuiProgress(unsigned units = 1);
		void UpdateGuiCaption(std::string_view currFileName);
		void AdvanceAndUpdateGui(std::string_view currFileName, unsigned units = 1);

	private:
		GuiController() = default;
		~GuiController() = default;

		void SetUnitsDone(unsigned units);

		unsigned m_total_units { 0 };
		unsigned m_units_done { 0 };
	};

}