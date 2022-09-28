#include "Gui.h"

namespace gui {

	bool Gui::OnInit(void) {
		m_frame = std::make_unique< wxFrame >(nullptr, wxID_ANY, wxT(""));
		GetFrame()->Show(false);
		SetTopWindow(GetFrame());

		m_dialog = std::make_unique< wxProgressDialog >(
			GetTitle(),
			GetCaption(),
			100, 
			GetFrame(),
			wxPD_AUTO_HIDE | wxPD_APP_MODAL | wxPD_CAN_ABORT |
			wxPD_ELAPSED_TIME | wxPD_ESTIMATED_TIME | wxPD_REMAINING_TIME | 
			wxPD_SMOOTH
			);
		m_dialog->SetWindowStyleFlag(wxRESIZE_BORDER);

		assert(GetFrame());
		assert(GetDialog());
		return true;
	}

	int Gui::OnExit(void) {
		m_dialog.reset();
		m_frame.reset();
		return 0;
	}

	void Gui::Render(void) {
		assert(GetDialog());

		if (GetDialog()->WasCancelled()) EmitCancel();
		GetDialog()->Update(GetProgress(), GetCaption());
	}

}