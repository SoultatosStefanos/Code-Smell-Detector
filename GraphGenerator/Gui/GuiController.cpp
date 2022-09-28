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
		const auto percentageDone = (GetUnitsDone() * 100) / GetTotalUnits();
		GetGui().SetProgress(percentageDone);
	}

	void GuiController::UpdateGuiCaption(std::string_view currFileName) {
		auto&& caption = std::string(currFileName) + '\t' + std::to_string(GetUnitsDone()) + "/" + std::to_string(GetTotalUnits());
		GetGui().SetCaption(std::move(caption));
	}

	void GuiController::AdvanceAndUpdateGui(std::string_view currFileName, unsigned units) { 
		AdvanceGuiProgress(units); 
		UpdateGuiCaption(currFileName); 
	}

	void GuiController::SetUnitsDone(unsigned units) {
		assert(GetTotalUnits() >= GetUnitsDone());
		assert(units <= GetTotalUnits());
		m_units_done = units;
		assert(GetTotalUnits() >= GetUnitsDone());
	}

}