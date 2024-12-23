#include <wx/wx.h>
#include "App.h"
#include "RemoteControlFrame.h"

wxIMPLEMENT_APP(App);

bool App::OnInit()
{
	RemoteControlFrame *mainFrame = new RemoteControlFrame("Remote Control via Gmail");
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
