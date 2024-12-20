#include "App.h"
#include "RemoteControlDraft.h"
#include <wx/wx.h>

wxIMPLEMENT_APP(App);

bool App::OnInit()
{
	wxInitAllImageHandlers();

	RemoteControlDraft *mainFrame = new RemoteControlDraft("Remote Control via Gmail");
	mainFrame->SetClientSize(550, 400);
	mainFrame->Center();

	if (!mainFrame)
	{
		wxLogError("Failed to create the main frame.");
		return false;
	}

	mainFrame->Show();
	return true;
}
