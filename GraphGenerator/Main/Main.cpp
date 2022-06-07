#include "ArchitectureMining.h"
#include "ProgressBar.h"

int main(int argc, char* argv[]) { // FIXME Return values

#ifdef PROGRESS_BAR

	wxEntryStart(argc, argv);

	wxGetApp().SetMax(17);
  	wxTheApp->CallOnInit();
	  
	auto res = arch::MineArchitecture(argc, argv);

	//wxGetApp().Finished();

  	wxTheApp->OnRun();

	return res;

#else

	return arch::MineArchitecture(argc, argv);

#endif

}