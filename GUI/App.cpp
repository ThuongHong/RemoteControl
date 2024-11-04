#include "App.h"
#include "MainFrame.h"
#include "RemoteControlDraft.h"
#include <wx/wx.h>

wxIMPLEMENT_APP(App);

bool App::OnInit() {
	RemoteControlDraft* mainFrame = new RemoteControlDraft("C++ GUI");
	mainFrame->SetClientSize(550, 350);
	mainFrame->Center();
	mainFrame->Show();
	return true;
}
 