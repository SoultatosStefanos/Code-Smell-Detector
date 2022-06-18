#include "Gui.h"

IMPLEMENT_APP_NO_MAIN(gui::Gui);

namespace gui {

bool Gui::OnInit() {
  assert(m_on_cancel);

  m_max_str = std::to_string(m_max);

  m_frame = new wxFrame(NULL, wxID_ANY, wxT("Architecture Mining"));
  this->SetTopWindow(m_frame);
  m_frame->Show(false);

  m_dialog = new wxProgressDialog(wxT("Mining..."), wxT("Keep waiting..."), m_max, m_frame, wxPD_AUTO_HIDE | wxPD_APP_MODAL | wxPD_CAN_ABORT |
                                                                                           wxPD_ELAPSED_TIME | wxPD_ESTIMATED_TIME | wxPD_REMAINING_TIME);
  assert(m_max != 0);

  return true;
}

void Gui::Update() {
  assert(m_frame != nullptr);
  assert(m_dialog != nullptr);
  assert(m_on_cancel);

  if (m_dialog->WasCancelled()) 
    Cancel();

  UpdateProgressBar();
}

void Gui::Update(const char* file) {
  assert(m_frame != nullptr);
  assert(m_dialog != nullptr);
  assert(m_on_cancel);

  if (m_dialog->WasCancelled()) 
    Cancel();

  if (m_current_file != file) {
    m_current_file = file;
    ++m_curr;
  }

  UpdateProgressBar();
}

void Gui::Finish() {
  m_dialog->Update(m_max, "Finished Mining");
  delete m_dialog;
}

void Gui::UpdateProgressBar() {
  assert(m_curr <= m_max);

  m_progress_str = m_current_file + '\t' + std::to_string(m_curr) + '/' + m_max_str + '\t' + std::to_string(m_curr * 100 / m_max) + '%';
  m_dialog->Update(m_curr, m_progress_str);
}

void Gui::SkipFiles(size_t num, const std::string& currFile) {
  m_curr += num;
  m_current_file = currFile;
  UpdateProgressBar();
}

void Gui::Cancel() {
  assert(m_on_cancel);

  m_on_cancel();
}


int Gui::OnExit() {
  return 0;
}

}
