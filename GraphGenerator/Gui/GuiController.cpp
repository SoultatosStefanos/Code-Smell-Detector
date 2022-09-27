#include "GuiController.h"
#include "Gui.h"
#include <string>

IMPLEMENT_APP_NO_MAIN(gui::Gui);

namespace gui {

	GuiController& GuiController::GetSingleton() { 
		static auto singleton = GuiController();
		return singleton; 
	}

	const Gui& GuiController::GetGui(void) const {
		return wxGetApp();
	}

	Gui& GuiController::GetGui(void) {
		return wxGetApp();
	}

	void GuiController::AdvanceGuiProgress(unsigned units) {
		SetUnitsDone(GetUnitsDone() + units);
		UpdateGuiProgress();
	}

	void GuiController::SetUnitsDone(unsigned units) {
		assert(GetTotalUnits() >= GetUnitsDone());
		assert(units <= GetTotalUnits());
		m_units_done = units;
		assert(GetTotalUnits() >= GetUnitsDone());
	}

	void GuiController::UpdateGuiProgress(void) {
		const auto percentageDone = (GetUnitsDone() * 100) / GetTotalUnits();
		auto&& caption = "Parsed files: " + std::to_string(GetUnitsDone()) + "/" + std::to_string(GetTotalUnits());
		
		GetGui().SetProgress(percentageDone);
		GetGui().SetCaption(std::move(caption));
	}

}