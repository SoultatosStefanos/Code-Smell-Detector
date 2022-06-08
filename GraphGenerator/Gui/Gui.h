#pragma once

#include <wx/wx.h>
#include <wx/progdlg.h>

namespace gui {

class Gui : public wxApp {
public:
  using FileObserver = std::function<std::string(void)>;

public:
  bool OnInit(void) override;
  int OnExit(void) override;

  void Update(void);
  void Update(const std::string& file);
  void Finished(void);

  void SetMax(size_t _max) { m_max = _max; }
  void SetFileObserver(const FileObserver& f) { m_file_obs = f; }

private:
  size_t m_max = 0;
  long m_curr = -1;
  wxProgressDialog* m_dialog = nullptr;
  wxFrame* m_frame = nullptr;
  std::string m_max_str = "";
  std::string m_current_file = "";
  std::string m_progress_str = "";

  FileObserver m_file_obs;

};

}

DECLARE_APP(gui::Gui);