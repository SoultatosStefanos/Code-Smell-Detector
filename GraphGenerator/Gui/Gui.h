#pragma once

#include <wx/wx.h>
#include <wx/progdlg.h>
#include <boost/signals2/signal.hpp>
#include <memory>
#include <string>

namespace gui {

	class Gui : public wxApp {
	private:
		using CancelSignal = boost::signals2::signal< void() >;

	public:
		using CancelSlot = CancelSignal::slot_type;
		using Connection = boost::signals2::connection;

	public:
		Gui() = default;
		virtual ~Gui() override = default;

		unsigned GetProgress(void) const { return m_progress; } 
		void SetProgress(unsigned curr) { m_progress = curr; }

		const std::string& GetCaption(void) const { return m_caption; }
		void SetCaption(std::string caption) { m_caption = std::move(caption); }

		const std::string& GetTitle(void) const { return m_title; }
		void SetTitle(std::string title) { m_title = std::move(title); }

		bool OnInit(void) override;
		int OnExit(void) override;

		void Render(void);

		Connection ConnectToCancel(const CancelSlot& f) { return m_cancel_signal.connect(f); }

	protected:
		wxProgressDialog* GetDialog() const { return m_dialog.get(); }
		wxFrame* GetFrame() const { return m_frame.get(); }

		void EmitCancel(void) const { m_cancel_signal(); }

	private:
		unsigned m_progress { 0 };
		std::string m_caption { "Parsed files: " }; 
		std::string m_title { "Mining..." }; 
		std::unique_ptr< wxProgressDialog > m_dialog;
		std::unique_ptr< wxFrame > m_frame;

		CancelSignal m_cancel_signal;
	};

}

DECLARE_APP(gui::Gui);
