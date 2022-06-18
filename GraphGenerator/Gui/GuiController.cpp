// GUI-Clang bridge.
// Soultatos Stefanos 2022

#include <cassert>

#include "GuiController.h"

namespace gui {

	GuiController& GuiController::GetSingleton() {
		static GuiController singleton;
		return singleton;
	}

	void GuiController::UpdateGui(const MatchResult& res, const Decl& decl) const {
		assert(m_is_header);
		assert(m_is_ignored);
	
		const auto srcLocation = res.SourceManager->getPresumedLoc(decl.getLocation());
	
		if (srcLocation.isValid() and !m_is_ignored(srcLocation.getFilename()) and !m_is_header(srcLocation.getFilename()))
			wxGetApp().Update(srcLocation.getFilename());
		else
			wxGetApp().Update();
	}

} // gui