#include "./ProgressBar.h"

bool ProgressBar::OnInit() {
  int i; // Remove
  SetMax(10); // Remove
  m_max_str = std::to_string(m_max);

  m_file_obs = []() -> std::string { // Remove
    static std::string file = "";
    file += 'a';
    return std::string(file + ".cpp");
  };

  m_frame = new wxFrame(NULL, wxID_ANY, wxT("Architecture Mining"));
  this->SetTopWindow(m_frame);
  m_frame->Show(true);

  m_dialog = new wxProgressDialog(wxT("Mining..."), wxT("Keep waiting..."), m_max, m_frame, wxPD_AUTO_HIDE | wxPD_APP_MODAL);

  assert(m_max != 0);
  for (int i = 0; i < m_max; i++) { // Remove
    Update();
    wxMilliSleep(2000);
  }
  Finished(); // Remove

  return true;
}

void ProgressBar::Update(void) {
  assert(m_frame != nullptr);
  assert(m_dialog != nullptr);

  auto file = m_file_obs();
  if (m_current_file != file) {
    m_current_file = file;
    ++m_curr;
    m_progress_str = m_current_file + '\n' + std::to_string(m_curr) + '/' + m_max_str;
    m_dialog->Update(m_curr, m_progress_str);
  }
}

void ProgressBar::Update(const std::string& file) {
  assert(m_dialog != nullptr);
  assert(m_frame != nullptr);

  if (m_current_file != file) {
    m_current_file = file;
    ++m_curr;
    m_progress_str = m_current_file + '\n' + std::to_string(m_curr) + '/' + m_max_str;
    m_dialog->Update(m_curr, m_progress_str);
  }
}

void ProgressBar::Finished(void) {
  m_dialog->Update(m_max, "Finished Mining");
  delete m_dialog;
}

int ProgressBar::OnExit() {
  return 0;
}
