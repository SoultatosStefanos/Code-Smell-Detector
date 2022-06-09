#include "Gui.h"

IMPLEMENT_APP_NO_MAIN(gui::Gui);

namespace gui {

bool Gui::OnInit() {
  m_max_str = std::to_string(m_max);

  m_frame = new wxFrame(NULL, wxID_ANY, wxT("Architecture Mining"));
  this->SetTopWindow(m_frame);
  m_frame->Show(true);

  m_dialog = new wxProgressDialog(wxT("Mining..."), wxT("Keep waiting..."), m_max, m_frame, wxPD_AUTO_HIDE | wxPD_APP_MODAL | wxPD_CAN_ABORT | wxPD_REMAINING_TIME);

  assert(m_max != 0);

  return true;
}

void Gui::Update(void) {
  assert(m_frame != nullptr);
  assert(m_dialog != nullptr);
  assert(m_file_obs);
  assert(m_on_finish);
  
  auto file = m_file_obs();
  if (m_current_file != file) {
    m_current_file = file;
    ++m_curr;
    m_progress_str = m_current_file + '\n' + std::to_string(m_curr) + '/' + m_max_str;
    m_dialog->Update(m_curr, m_progress_str);
  }
}

void Gui::Update(const std::string& file) {
  assert(m_dialog != nullptr);
  assert(m_frame != nullptr);
  assert(m_on_finish);

  if (m_current_file != file) {
    m_current_file = file;
    ++m_curr;

    assert(m_curr < static_cast<decltype(m_curr)>(m_max));

    m_progress_str = m_current_file + '\n' + std::to_string(m_curr) + '/' + m_max_str + '\t' + std::to_string(m_curr * 100 / m_max) + '%';
    if (!m_dialog->Update(m_curr, m_progress_str)) // check if the cancel button was pressed
      m_on_finish();

  }
}

void Gui::Finished(void) {
  m_dialog->Update(m_max, "Finished Mining");
  if (!m_dialog->WasCancelled()) // check if program finished without being cancelled
    m_on_finish();
  delete m_dialog;
}

int Gui::OnExit() {
  return 0;
}

}
