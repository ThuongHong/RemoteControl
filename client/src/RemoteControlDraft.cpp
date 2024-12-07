#include "RemoteControlDraft.h"

RemoteControlDraft::RemoteControlDraft(const wxString& title) : wxFrame(nullptr, wxID_ANY, title) {
	wxInitAllImageHandlers();

	//Meta Data
	wxString description("Our program allows you to control computers through Gmail. Send commands, execute tasks, all from your Gmail inbox.");
	wxFont headerFont(35, wxFONTFAMILY_SWISS, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_NORMAL, false, "Impact"); //Unispace, Impact
	wxFont mainFont(11, wxFONTFAMILY_SWISS, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_NORMAL);
	wxFont processFont(28, wxFONTFAMILY_SWISS, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_NORMAL, false, "Impact");
	wxImage image("assets\\HCMUS.png", wxBITMAP_TYPE_PNG);
	wxImage icon("assets\\google_icon.png", wxBITMAP_TYPE_PNG);
	icon = icon.Scale(50, 50, wxIMAGE_QUALITY_HIGH);
	wxBitmap bitmap(icon);

	if (CreateEmailClient(client_id, client_secret, redirect_uri, gmailClient)) {
		std::cout << "Create email client successfully!" << std::endl;
		std::cout << "Client ID: " << gmailClient->m_client_id << std::endl;
		std::cout << "Client secret: " << gmailClient->m_client_secret << std::endl;
		std::cout << "Redirect URI: " << gmailClient->m_redirect_uri << std::endl;
	}

	/*Create Panel*/
	panelLogin = new PanelLogin(this, description, image, headerFont, mainFont, bitmap);
	panelAuthorization = new PanelAuthorization(this, headerFont, mainFont);
	panelRoles = new PanelRoles(this, description, headerFont, mainFont, bitmap);
	panelSender = new PanelSender(this, image, headerFont, mainFont);
	panelReceiver = new PanelReceiver(this, image, headerFont, mainFont, m_statusText);

	m_statusText = new wxStaticText(panelReceiver, wxID_ANY, "Initializing...");
	m_statusText->SetFont(processFont);

	panelAuthorization->Hide();
	panelRoles->Hide();
	panelSender->Hide();
	panelReceiver->Hide();

	panelLogin->BindControl(panelAuthorization);
	panelAuthorization->BindControl(panelRoles, authorization_code, access_token, refresh_token, client, gmailClient);
	panelRoles->BindControl(panelSender, panelReceiver, ip_address, port, target_email, m_statusText, client);
	panelSender->BindControl(filename, processID);
	panelReceiver->BindControl(client);

	panelReceiver->CreateSizer(m_statusText);

	/* Create Sizer */
	sizerMain = new wxBoxSizer(wxVERTICAL);
	sizerMain->Add(panelLogin, 1, wxEXPAND);
	sizerMain->Add(panelAuthorization, 1, wxEXPAND);
	sizerMain->Add(panelRoles, 1, wxEXPAND);
	sizerMain->Add(panelSender, 1, wxEXPAND);
	sizerMain->Add(panelReceiver, 1, wxEXPAND);

	this->SetSizer(sizerMain);
}

bool RemoteControlDraft::CreateEmailClient(const std::string& client_id, const std::string& client_secret, const std::string& redirect_uri, wxScopedPtr<GmailClient>& gmailClient)
{
	gmailClient.reset(new GmailClient(client_id, client_secret, redirect_uri));
	if (gmailClient) return true;
	return false;
}

// Todo: OnPanelLoginButtonClicked -> Login Gmail, OnButtonExitClicked -> Close socket, OnPanelSenderButtonConfirm -> Process command (Send email), OnPanelAuthorizationEventListened -> Listen to command,