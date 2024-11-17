#include "RemoteControlDraft.h"

RemoteControlDraft::RemoteControlDraft(const wxString& title) : wxFrame(nullptr, wxID_ANY, title) {
	wxInitAllImageHandlers();

	//Meta Data
	wxString description("Our program allows you to manage, control computers through email. Send commands, execute tasks, and monitor system activities, all from your Gmail inbox.");
	wxFont headerFont(35, wxFONTFAMILY_SWISS, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_NORMAL, false, "Impact"); //Unispace, Impact
	wxFont mainFont(11, wxFONTFAMILY_SWISS, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_NORMAL);
	wxImage image("assets\\HCMUS.png", wxBITMAP_TYPE_PNG);
	wxImage icon("assets\\google_icon.png", wxBITMAP_TYPE_PNG);
	icon = icon.Scale(50, 50, wxIMAGE_QUALITY_HIGH);
	wxBitmap bitmap(icon);

	/*Create Panel*/
	panelLogin = new PanelLogin(this, description, image, headerFont, mainFont, bitmap);
	panelRoles = new PanelRoles(this, description, headerFont, mainFont, bitmap);
	panelAuthorization = new PanelAuthorization(this, headerFont, mainFont);
	panelSender = new PanelSender(this, image, headerFont, mainFont);
	panelReceiver = new PanelReceiver(this, image, headerFont, mainFont);

	panelRoles->Hide();
	panelSender->Hide();
	panelAuthorization->Hide();
	panelReceiver->Hide();

	// wxWebView* webView = wxWebView::New(this, wxID_ANY, "https://mail.google.com/", wxDefaultPosition, wxSize(800, 600));

	panelLogin->BindControl(panelRoles);
	panelRoles->BindControl(panelAuthorization, panelReceiver, ip_address, port, target_email, client);
	panelAuthorization->BindControl(panelSender, authorization_code);
	panelSender->BindControl();
	panelReceiver->BindControl();

	/* Create Sizer */
	sizerMain = new wxBoxSizer(wxVERTICAL);
	sizerMain->Add(panelLogin, 1, wxEXPAND);
	sizerMain->Add(panelRoles, 1, wxEXPAND);
	sizerMain->Add(panelAuthorization, 1, wxEXPAND);
	sizerMain->Add(panelSender, 1, wxEXPAND);
	sizerMain->Add(panelReceiver, 1, wxEXPAND);

	this->SetSizer(sizerMain);
}

// Todo: OnPanelLoginButtonClicked -> Login Gmail, OnButtonExitClicked -> Close socket, OnPanelSenderButtonConfirm -> Process command (Send email), OnPanelAuthorizationEventListened -> Listen to command,
