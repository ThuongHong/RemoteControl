#include "App.h"
#include "RemoteControlDraft.h"
#include <wx/wx.h>

wxIMPLEMENT_APP(App);

bool App::OnInit() {
	RemoteControlDraft* mainFrame = new RemoteControlDraft("C++ GUI");
	mainFrame->SetClientSize(550, 350);
	mainFrame->Center();

	if (!mainFrame) {
		wxLogError("Failed to create the main frame.");
		return false;
	}

	mainFrame->Show();
	return true;
}
 