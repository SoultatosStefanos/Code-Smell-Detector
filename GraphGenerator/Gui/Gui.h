#pragma once

#include <wx/wx.h>
#include <wx/progdlg.h>

namespace gui {

class Gui : public wxApp {
public:
  using OnCancel = std::function<void(void)>;

public:
  bool OnInit(void) override;
  int OnExit(void) override;

  void Update(void);
  void UpdateProgressBar(const std::string& file);
  void Finished(void);

  void SetMax(size_t _max) { m_max = _max; }
  void SetOnCancel(const OnCancel& f) { m_on_cancel = f; }

private:
  void Cancel();

  size_t m_max = 0;
  long m_curr = -1;
  wxProgressDialog* m_dialog = nullptr;
  wxFrame* m_frame = nullptr;
  std::string m_max_str = "";
  std::string m_current_file = "";
  std::string m_progress_str = "";

  OnCancel m_on_cancel;

};

}

DECLARE_APP(gui::Gui);