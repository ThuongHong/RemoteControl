#include "RemoteControlFrame.h"

RemoteControlFrame::RemoteControlFrame(const wxString &title) : wxFrame(nullptr, wxID_ANY, title, wxDefaultPosition, wxDefaultSize,
																		wxDEFAULT_FRAME_STYLE & ~wxMINIMIZE_BOX & ~wxMAXIMIZE_BOX & ~wxRESIZE_BORDER)
{
	// Allow wxApp to load images
	wxInitAllImageHandlers();

	wxString description("Our program allows you to control computers through Gmail. Send commands, execute tasks, all from your Gmail inbox.");
	wxFont headerFont(35, wxFONTFAMILY_SWISS, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_NORMAL, false, "Impact"); // Unispace, Impact
	wxFont mainFont(11, wxFONTFAMILY_SWISS, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_NORMAL);
	wxFont processFont(28, wxFONTFAMILY_SWISS, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_NORMAL, false, "Impact");

	// Get executable path
	wchar_t exePath[MAX_PATH];
	GetModuleFileNameW(NULL, exePath, MAX_PATH);
	std::wstring exeDir = std::wstring(exePath);
	exeDir = exeDir.substr(0, exeDir.find_last_of(L"\\/"));

	// Load images with full path and error checking
	wxString hcmusPath = wxString(exeDir) + "\\assets\\HCMUS.png";
	wxString iconPath = wxString(exeDir) + "\\assets\\google_icon.png";

	wxImage image(hcmusPath, wxBITMAP_TYPE_PNG);
	wxImage icon(iconPath, wxBITMAP_TYPE_PNG);

	icon = icon.Scale(50, 50, wxIMAGE_QUALITY_HIGH);
	wxBitmap bitmap(icon);

	// Create OAuth Handler
	CreateOAuth2Handler(client_id, client_secret, redirect_uri, oAuth2Handler);

	// Create Panel
	panelLogin = new PanelLogin(this, description, image, headerFont, mainFont, bitmap);
	panelRoles = new PanelRoles(this, description, headerFont, mainFont, bitmap);
	panelSender = new PanelSender(this, image, headerFont, mainFont);
	panelReceiver = new PanelReceiver(this, image, headerFont, mainFont, m_statusText);
	panelExplorer = new PanelExplorer(this, processID);
	m_statusText = new wxStaticText(panelReceiver, wxID_ANY, "Initializing...");

	// Set
	m_statusText->SetFont(processFont);
	panelRoles->Hide();
	panelSender->Hide();
	panelReceiver->Hide();
	panelExplorer->Hide();

	// Bind event
	panelLogin->BindControl(panelRoles, redirect_uri, client_id, access_token, refresh_token, oAuth2Handler);
	panelRoles->BindControl(panelSender, panelReceiver, ip_address, port, receive_email, send_email, m_statusText, client, access_token, gmailSender, tasks, gmailReceiver);
	panelSender->BindControl(panelExplorer, file_name, app_svc_name, processID, receive_email, gmailSender);
	panelReceiver->BindControl(client);
	panelExplorer->BindControl(panelSender, processID, file_name, gmailSender);
	BindControl(client);

	panelReceiver->CreateSizer(m_statusText);

	// Create Sizer
	wxBoxSizer *sizerMain = new wxBoxSizer(wxVERTICAL);
	sizerMain->Add(panelLogin, 1, wxEXPAND);
	sizerMain->Add(panelRoles, 1, wxEXPAND);
	sizerMain->Add(panelSender, 1, wxEXPAND);
	sizerMain->Add(panelReceiver, 1, wxEXPAND);
	sizerMain->Add(panelExplorer, 1, wxEXPAND);

	this->SetSizer(sizerMain);
}

void RemoteControlFrame::BindControl(wxScopedPtr<Client> &client)
{
	Bind(wxEVT_CLOSE_WINDOW, [this, &client](wxCloseEvent &evt)
		 {
		if (client) client->sendString("end");
		evt.Skip(); });
}

bool RemoteControlFrame::CreateOAuth2Handler(const std::string &client_id, const std::string &client_secret, const std::string &redirect_uri, wxScopedPtr<OAuth2Handler> &oAuth2Handler)
{
	oAuth2Handler.reset(new OAuth2Handler(client_id, client_secret, redirect_uri));
	if (oAuth2Handler)
		return true;
	return false;
}