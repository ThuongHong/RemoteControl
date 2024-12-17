#include "panels.h"

PanelLogin::PanelLogin(wxWindow* parent, wxString description, wxImage image, wxFont headerFont, wxFont mainFont, wxBitmap bitmap) : wxPanel(parent, wxID_ANY)
{
	parent_ = parent;
	Create(description, image);
	Set(headerFont, mainFont, bitmap);
	CreateSizer();
}
void PanelLogin::Create(wxString description, wxImage image)
{
	Image = image.Scale(180, 180, wxIMAGE_QUALITY_HIGH);
	roles = { "Sender", "Receiver" };
	TextTitle = new wxStaticText(this, wxID_ANY, "PC REMOTE CONTROL", wxDefaultPosition, wxSize(600, -1), wxALIGN_CENTER);
	TextDescription = new wxStaticText(this, wxID_ANY, description);
	TextMem1 = new wxStaticText(this, wxID_ANY, "Luu Thuong Hong - 23122006");
	TextMem2 = new wxStaticText(this, wxID_ANY, "Nguyen Tan Hung - 23122007");
	TextMem3 = new wxStaticText(this, wxID_ANY, "Chung Tin Dat      - 23122024");
	ButtonLogin = new wxButton(this, wxID_ANY, "Login with Google", wxDefaultPosition, wxSize(280, 80));
	Logo = new wxStaticBitmap(this, wxID_ANY, wxBitmap(Image));
}
void PanelLogin::Set(wxFont headerFont, wxFont mainFont, wxBitmap bitmap)
{
	wxFont buttonFont(15, wxFONTFAMILY_SWISS, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_NORMAL);
	TextTitle->SetFont(headerFont);
	TextTitle->SetBackgroundColour(wxColour(187, 189, 228));
	TextDescription->SetFont(mainFont);
	TextDescription->Wrap(220);
	TextMem1->SetFont(mainFont);
	TextMem2->SetFont(mainFont);
	TextMem3->SetFont(mainFont);
	ButtonLogin->SetFont(buttonFont);
	ButtonLogin->SetBitmap(bitmap);
	ButtonLogin->SetForegroundColour(wxColour(128, 128, 128));
	ButtonLogin->SetBitmapPosition(wxLEFT);
	ButtonLogin->SetBitmapMargins(20, 0);
}
void PanelLogin::CreateSizer()
{
	MainSizer = new wxBoxSizer(wxVERTICAL);
	SubSizer1 = new wxBoxSizer(wxHORIZONTAL);
	SubSizer2 = new wxBoxSizer(wxHORIZONTAL);
	SubSizer3 = new wxBoxSizer(wxVERTICAL);

	SubSizer1->Add(Logo, 0, wxLEFT, 20);
	SubSizer1->Add(ButtonLogin, 0, wxALIGN_CENTER_VERTICAL | wxLEFT, 35);

	SubSizer3->Add(TextMem1);
	SubSizer3->Add(TextMem2, 0, wxTOP, 10);
	SubSizer3->Add(TextMem3, 0, wxTOP, 10);

	SubSizer2->Add(TextDescription, 0, wxLEFT, 20);
	SubSizer2->Add(SubSizer3, 0, wxLEFT, 50);

	MainSizer->Add(TextTitle, 0, wxBOTTOM | wxTOP, 10);
	MainSizer->Add(SubSizer2, 0, wxTOP, 20);
	MainSizer->Add(SubSizer1, 0, wxBOTTOM, 20);

	this->SetSizer(MainSizer);
}
void PanelLogin::BindControl(wxPanel* desPanel, std::string redirect_uri, std::string client_id, std::string &access_token, std::string &refresh_token, wxScopedPtr<OAuth2Handler> &oAuth2Handler) {
	ButtonLogin->Bind(wxEVT_BUTTON, [this, desPanel, redirect_uri, client_id, &access_token, &refresh_token, &oAuth2Handler](wxCommandEvent&) {
		OnButtonClicked(desPanel, redirect_uri, client_id, access_token, refresh_token, oAuth2Handler);
	});
}
void PanelLogin::OnButtonClicked(wxPanel* desPanel, std::string redirect_uri, std::string client_id, std::string &access_token, std::string &refresh_token, wxScopedPtr<OAuth2Handler> &oAuth2Handler)
{
	if (curl_global_init(CURL_GLOBAL_DEFAULT) != 0)
	{
		wxMessageBox("Failed to initialize CURL", "Error", wxOK | wxICON_ERROR);
	}

	wxString auth_url =
		"https://accounts.google.com/o/oauth2/v2/auth?"
		"scope=https%3A%2F%2Fwww.googleapis.com%2Fauth%2Fgmail.modify"
		"&access_type=offline"
		"&include_granted_scopes=true"
		"&response_type=code"
		"&redirect_uri=" +
		redirect_uri +
		"&client_id=" + client_id;

	if (!wxLaunchDefaultBrowser(auth_url))
	{
		wxMessageBox("Failed to open the browser.", "Error", wxOK | wxICON_ERROR);
		curl_global_cleanup();
	}

	try
	{
		HttpListener listener(8080);
		std::string authorization_code = listener.waitForAuthorizationCode();

		if (authorization_code.empty())
		{
			wxMessageBox("Failed to get authorization code", "Error", wxOK | wxICON_ERROR);
			curl_global_cleanup();
		}

		// Exchange authorization code for tokens
		std::string refresh_token;
		if (oAuth2Handler->exchangeAuthCodeForAccessToken(authorization_code, access_token, refresh_token))
		{
			oAuth2Handler->saveAccessTokenToFile(access_token);
			std::cout << "Access token: " << access_token << std::endl;
			wxMessageBox("Access token exchanged and saved successfully!");
		}
		else
		{
			wxMessageBox("Failed to exchange authorization code for access token.", "Error", wxOK | wxICON_ERROR);
			curl_global_cleanup();
		}
	}
	catch (const std::exception& e)
	{
		wxMessageBox(wxString::Format("HTTP Listener error: %s", e.what()), "Error", wxOK | wxICON_ERROR);
		curl_global_cleanup();
	}

	this->Hide();
	desPanel->Show();
	desPanel->Layout();
	parent_->Layout();
}

PanelRoles::PanelRoles(wxWindow* parent, wxString description, wxFont headerFont, wxFont mainFont, wxBitmap bitmap) : wxPanel(parent, wxID_ANY)
{
	parent_ = parent;
	Create(description);
	Set(headerFont, mainFont);
	CreateSizer();
}
void PanelRoles::Create(wxString description)
{
	roles = { "Sender", "Receiver" };
	TextTitle = new wxStaticText(this, wxID_ANY, "PC REMOTE CONTROL", wxDefaultPosition, wxSize(600, -1), wxALIGN_CENTER);
	TextDescription = new wxStaticText(this, wxID_ANY, description);
	TextMem1 = new wxStaticText(this, wxID_ANY, "Luu Thuong Hong - 23122006");
	TextMem2 = new wxStaticText(this, wxID_ANY, "Nguyen Tan Hung - 23122007");
	TextMem3 = new wxStaticText(this, wxID_ANY, "Chung Tin Dat      - 23122024");
	TextEmail = new wxStaticText(this, wxID_ANY, "Receiver Email");
	TextIP = new wxStaticText(this, wxID_ANY, "Server IP");
	TextPort = new wxStaticText(this, wxID_ANY, "Port");
	Roles = new wxRadioBox(this, wxID_ANY, "Choose your role:", wxDefaultPosition, wxDefaultSize, roles, 2, wxRA_SPECIFY_ROWS);
	InputFieldEmail = new wxTextCtrl(this, wxID_ANY, "", wxDefaultPosition, wxSize(200, -1));
	InputFieldIP = new wxTextCtrl(this, wxID_ANY, "", wxDefaultPosition, wxSize(200, -1));
	InputFieldPort = new wxTextCtrl(this, wxID_ANY, "", wxDefaultPosition, wxSize(200, -1));
	ButtonConfirm = new wxButton(this, wxID_ANY, "CONFIRM", wxDefaultPosition, wxSize(150, -1));
}
void PanelRoles::Set(wxFont headerFont, wxFont mainFont)
{
	TextTitle->SetFont(headerFont);
	TextTitle->SetBackgroundColour(wxColour(187, 189, 228));
	TextDescription->SetFont(mainFont);
	TextDescription->Wrap(220);
	TextMem1->SetFont(mainFont);
	TextMem2->SetFont(mainFont);
	TextMem3->SetFont(mainFont);
	Roles->SetFont(mainFont);
	TextEmail->SetFont(mainFont);
	TextIP->SetFont(mainFont);
	TextPort->SetFont(mainFont);
	TextIP->Hide();
	TextPort->Hide();
	InputFieldIP->Hide();
	InputFieldPort->Hide();
	ButtonConfirm->SetFont(mainFont);
	ButtonConfirm->Disable();
}
void PanelRoles::CreateSizer()
{
	MainSizer = new wxBoxSizer(wxVERTICAL);
	SubSizer1 = new wxBoxSizer(wxHORIZONTAL);
	SubSizer2 = new wxBoxSizer(wxVERTICAL);
	SubSizer3 = new wxBoxSizer(wxHORIZONTAL);
	SubSizer4 = new wxBoxSizer(wxVERTICAL);

	SubSizer2->Add(TextEmail, 0, wxALIGN_CENTER_HORIZONTAL | wxBOTTOM, 5);
	SubSizer2->Add(InputFieldEmail, 0, wxEXPAND | wxBOTTOM, 5);
	SubSizer2->Add(TextIP, 0, wxALIGN_CENTER_HORIZONTAL | wxBOTTOM, 5);
	SubSizer2->Add(InputFieldIP, 0, wxEXPAND | wxBOTTOM, 5);
	SubSizer2->Add(TextPort, 0, wxALIGN_CENTER_HORIZONTAL | wxBOTTOM, 5);
	SubSizer2->Add(InputFieldPort, 0, wxEXPAND);

	SubSizer1->Add(Roles, 0, wxALIGN_CENTER_VERTICAL | wxLEFT, 80);
	SubSizer1->AddSpacer(80);
	SubSizer1->Add(SubSizer2, 0, wxALIGN_CENTER_VERTICAL);

	SubSizer4->Add(TextMem1);
	SubSizer4->Add(TextMem2, 0, wxTOP, 10);
	SubSizer4->Add(TextMem3, 0, wxTOP, 10);

	SubSizer3->Add(TextDescription, 0, wxLEFT, 20);
	SubSizer3->Add(SubSizer4, 0, wxLEFT, 50);

	MainSizer->Add(TextTitle, 0, wxALIGN_CENTER_HORIZONTAL | wxBOTTOM | wxTOP, 10);
	MainSizer->Add(SubSizer3, 0, wxTOP, 20);
	MainSizer->AddSpacer(10);
	MainSizer->Add(SubSizer1);
	MainSizer->Add(ButtonConfirm, 0, wxALIGN_CENTER_HORIZONTAL | wxTOP, 20);

	this->SetSizer(MainSizer);
}
void PanelRoles::BindControl(wxPanel* desPanel1, wxPanel* desPanel2, std::string& ip_address, int &port, std::string &receive_email, std::string &send_email, wxStaticText* m_statusText, wxScopedPtr<Client> &client, std::string &access_token, wxScopedPtr<GmailSender>& gmailSender, std::vector<std::string> &tasks, wxScopedPtr<GmailReceiver> &gmailReceiver)
{
	Roles->Bind(wxEVT_RADIOBOX, &PanelRoles::OnRolesChanged, this);
	ButtonConfirm->Bind(wxEVT_BUTTON, [this, desPanel1, desPanel2, &ip_address, &port, &receive_email, &send_email, m_statusText, &client, &access_token, &gmailSender, &tasks, &gmailReceiver](wxCommandEvent&) {
		OnButtonClicked(desPanel1, desPanel2, ip_address, port, receive_email, send_email, m_statusText, client, access_token, gmailSender, tasks, gmailReceiver);
	});

	InputFieldEmail->Bind(wxEVT_TEXT, &PanelRoles::OnTextCtrlChanged, this);
	InputFieldIP->Bind(wxEVT_TEXT, &PanelRoles::OnTextCtrlChanged, this);
	InputFieldPort->Bind(wxEVT_TEXT, &PanelRoles::OnTextCtrlChanged, this);
}
bool PanelRoles::CreateClient(std::string& ip_address, int& port, wxStaticText* m_statusText, wxScopedPtr<Client>& client, std::vector<std::string> &tasks)
{
	client.reset(new Client(ip_address, port, m_statusText, tasks));
	if (client) return true;
	return false;
}
bool PanelRoles::CreateEmailSender(const std::string& access_token, wxScopedPtr<GmailSender>& gmailSender)
{
	gmailSender.reset(new GmailSender());
	gmailSender->setAccessToken(access_token);
	if (gmailSender) return true;
	return false;
}
bool PanelRoles::CreateEmailReceiver(const std::string& access_token, wxScopedPtr<GmailReceiver>& gmailReceiver)
{
	gmailReceiver.reset(new GmailReceiver());
	gmailReceiver->setAccessToken(access_token);
	return false;
}
void PanelRoles::OnButtonClicked(wxPanel* desPanel1, wxPanel* desPanel2, std::string& ip_address, int& port, std::string &receive_email, std::string &send_email, wxStaticText* m_statusText, wxScopedPtr<Client>& client, std::string &access_token, wxScopedPtr<GmailSender> &gmailSender, std::vector<std::string> &tasks, wxScopedPtr<GmailReceiver> &gmailReceiver)
{
	int selection = Roles->GetSelection();
	switch (selection) { 
	case 0: // To SENDER PANEL
		receive_email = InputFieldEmail->GetValue().ToStdString();
		/*std::cout << receive_email << std::endl;*/

		if (CreateEmailSender(access_token, gmailSender)) std::cout << "Create gmail sender successfully!" << std::endl;
		gmailSender->setTo(receive_email);

		this->Hide();
		desPanel1->Show();
		desPanel1->Layout();
		break;
	case 1: // To RECEIVER PANEL
		//Connect receiver to server
		this->Hide();
		desPanel2->Show();
		desPanel2->Layout();

		send_email = InputFieldEmail->GetValue().ToStdString();
		ip_address = InputFieldIP->GetValue().ToStdString();
		port = std::stoi(InputFieldPort->GetValue().ToStdString());

		if (CreateClient(ip_address, port, m_statusText, client, tasks)) {
			std::cout << "Create client successfully!" << std::endl;
			std::cout << "Client IP: " << client->ip_address_ << std::endl;
			std::cout << "Client port: " << client->port_ << std::endl;
		}

		if (CreateEmailReceiver(access_token, gmailReceiver)) std::cout << "Create gmail receiver successfully!" << std::endl;

		if (client->initialize(m_statusText)) {
			client->BindControl(m_statusText, send_email, gmailReceiver);
			std::cout << "Initialize client successfully!" << std::endl;
		}
		else {
			std::cout << "Error initialize client" << std::endl;
		}
		break;
	}
	std::cout << "Receive email: " << receive_email << std::endl;
	std::cout << "Send email: " << send_email << std::endl;
	parent_->Layout();
}
void PanelRoles::OnRolesChanged(wxCommandEvent& evt)
{
	int selection = evt.GetInt();
	TextIP->Hide();
	TextPort->Hide();
	InputFieldIP->Hide();
	InputFieldPort->Hide();

	InputFieldEmail->Clear();
	InputFieldIP->Clear();
	InputFieldPort->Clear();
	switch (selection) {
	case 0:
		TextEmail->SetLabel("Receiver Email");
		break;
	case 1:
		TextEmail->SetLabel("Sender Email");
		TextIP->Show();
		InputFieldIP->Show();
		TextPort->Show();
		InputFieldPort->Show();
		break;
	}
	this->Layout();
}
void PanelRoles::OnTextCtrlChanged(wxCommandEvent& event)
{
	bool enableButton = false;

	if (Roles->GetSelection() == 0) {
		wxString textEmail = InputFieldEmail->GetValue();
		enableButton = IsEmailFormat(textEmail);
	}
	else if (Roles->GetSelection() == 1) {
		wxString textEmail = InputFieldEmail->GetValue();
		wxString textIP = InputFieldIP->GetValue();
		wxString textPort = InputFieldPort->GetValue();
		enableButton = IsEmailFormat(textEmail) && IsIPFormat(textIP) && IsPortFormat(textPort);
	}

	ButtonConfirm->Enable(enableButton);
}
bool PanelRoles::IsEmailFormat(const wxString& text)
{
	const std::regex emailPattern(R"((\w+)(\.\w+)*@gmail\.com)");
	return std::regex_match(text.ToStdString(), emailPattern);
}
bool PanelRoles::IsIPFormat(const wxString& text)
{
	const std::regex ipPattern("^((25[0-5]|2[0-4][0-9]|[01]?[0-9][0-9]?)\\.){3}(25[0-5]|2[0-4][0-9]|[01]?[0-9][0-9]?)$");
	return std::regex_match(text.ToStdString(), ipPattern);
}
bool PanelRoles::IsPortFormat(const wxString& text)
{
	// Regular expression to check for a numeric string
	std::regex portRegex("^[0-9]+$");

	// Check if it matches the regex
	if (!std::regex_match(text.ToStdString(), portRegex)) {
		return false;
	}

	// Convert to an integer and check range
	int portNum = std::stoi(text.ToStdString());
	return portNum >= 0 && portNum <= 65535;
}

//PanelAuthorization::PanelAuthorization(wxWindow* parent, wxFont headerFont, wxFont mainFont) : wxPanel(parent, wxID_ANY)
//{
//	parent_ = parent;
//	Create();
//	Set(headerFont, mainFont);
//	CreateSizer();
//}
//void PanelAuthorization::BindControl(wxPanel* desPanel, std::string &authorization_code, std::string &access_token, std::string &refresh_token, wxScopedPtr<Client> &client, wxScopedPtr<GmailReceiver> &gmailReceiver)
//{
//	//ButtonYes->Bind(wxEVT_BUTTON, &PanelAuthorization::OnButtonYesClicked, this);
//	//ButtonNo->Bind(wxEVT_BUTTON, &PanelAuthorization::OnButtonNoClicked, this);
//	ButtonExit->Bind(wxEVT_BUTTON, &PanelAuthorization::OnButtonExitClicked, this);
//	ButtonConfirm->Bind(wxEVT_BUTTON, [this, desPanel, &authorization_code, &access_token, &refresh_token, &client, &gmailReceiver](wxCommandEvent&) {
//		OnButtonConfirmClicked(desPanel, authorization_code, access_token, refresh_token, client, gmailReceiver);
//	});
//	this->Bind(wxEVT_TIMER, &PanelAuthorization::OnClose, this, Timer->GetId());
//}
//void PanelAuthorization::Create()
//{
//	TextTitle = new wxStaticText(this, wxID_ANY, "PC REMOTE CONTROL", wxDefaultPosition, wxSize(600, -1), wxALIGN_CENTER);
//	TextHelp = new wxStaticText(this, wxID_ANY, "After logging in, please copy the authorization code from the URL and paste it here");
//	TextAuthorization = new wxStaticText(this, wxID_ANY, "Enter your authorization code:");
//	TextSuccess = new wxStaticText(this, wxID_ANY, "Access token exchanged and saved successfully!");
//	TextFail = new wxStaticText(this, wxID_ANY, "Failed to exchange authorization code for access token.");
//	ButtonConfirm = new wxButton(this, wxID_ANY, "Confirm");
//	ButtonExit = new wxButton(this, wxID_ANY, "Exit");
//	InputField = new wxTextCtrl(this, wxID_ANY, "", wxDefaultPosition, wxSize(300, -1));
//	Timer = new wxTimer(this);
//}
//void PanelAuthorization::Set(wxFont headerFont, wxFont mainFont)
//{
//	TextTitle->SetFont(headerFont);
//	TextTitle->SetBackgroundColour(wxColour(187, 189, 228));
//	TextHelp->SetFont(mainFont);
//	//TextAsk->SetFont(mainFont);
//	TextAuthorization->SetFont(mainFont);
//	TextSuccess->SetFont(mainFont);
//	TextFail->SetFont(mainFont);
//	//ButtonYes->SetFont(mainFont);
//	//ButtonNo->SetFont(mainFont);
//	ButtonConfirm->SetFont(mainFont);
//	ButtonExit->SetFont(mainFont);
//
//	//TextHelp->Hide();
//	//TextAuthorization->Hide();
//	TextSuccess->Hide();
//	TextFail->Hide();
//	//InputField->Hide();
//	//ButtonConfirm->Hide();
//	//ButtonExit->Hide();
//	//
//	//this->Bind(wxEVT_TIMER, &RemoteControlDraft::OnPanelAuthorizationClose, this);
//}
//void PanelAuthorization::CreateSizer()
//{
//	MainSizer = new wxBoxSizer(wxVERTICAL);
//	SubSizer1 = new wxBoxSizer(wxVERTICAL);
//	SubSizer2 = new wxBoxSizer(wxHORIZONTAL);
//	SubSizer3 = new wxBoxSizer(wxHORIZONTAL);
//
//	SubSizer3->Add(ButtonConfirm, 0, wxRIGHT, 30);
//	SubSizer3->Add(ButtonExit);
//
//	SubSizer1->Add(TextHelp, 0, wxALIGN_CENTER_HORIZONTAL | wxBOTTOM, 20);
//	SubSizer1->Add(TextAuthorization, 0, wxALIGN_CENTER_HORIZONTAL | wxBOTTOM, 20);
//	SubSizer1->Add(InputField, 0, wxALIGN_CENTER_HORIZONTAL | wxBOTTOM, 20);
//	SubSizer1->Add(TextSuccess, 0, wxALIGN_CENTER_HORIZONTAL);
//	SubSizer1->Add(TextFail, 0, wxALIGN_CENTER_HORIZONTAL);
//	SubSizer1->Add(SubSizer3, 0, wxALIGN_CENTER_HORIZONTAL);
//
//	SubSizer2->Add(SubSizer1, 1, wxALIGN_CENTER_VERTICAL);
//
//	MainSizer->Add(TextTitle, 0, wxALIGN_CENTER_HORIZONTAL | wxBOTTOM | wxTOP, 10);
//	MainSizer->Add(SubSizer2, 1, wxALIGN_CENTER_HORIZONTAL);
//
//	this->SetSizer(MainSizer);
//}
//void PanelAuthorization::OnButtonExitClicked(wxCommandEvent& evt)
//{
//	//Close socket
//	Timer->Stop();
//	parent_->Close(true);
//}
//void PanelAuthorization::OnButtonConfirmClicked(wxPanel* desPanel, std::string &authorization_code, std::string &access_token, std::string &refresh_token, wxScopedPtr<Client>& client, wxScopedPtr<GmailReceiver>& gmailReceiver) {
//	authorization_code = InputField->GetValue();
//	std::cout << authorization_code << std::endl;
//	if (gmailReceiver->exchangeAuthCodeForAccessToken(authorization_code, access_token, refresh_token))
//	{
//		// Save access token to file
//		gmailReceiver->saveAccessTokenToFile(access_token);
//		std::cout << "Access token exchanged and saved successfully!" << std::endl;
//		client->loadAccessToken();
//	}
//	else
//	{
//		std::cerr << "Failed to exchange authorization code for access token." << std::endl;
//	}
//	this->Hide();
//	desPanel->Show();
//	parent_->Layout();
//}
//void PanelAuthorization::OnClose(wxTimerEvent& evt) {
//	//Close socket
//	Timer->Stop();
//	parent_->Close(true);
//}

PanelSender::PanelSender(wxWindow* parent, wxImage image, wxFont headerFont, wxFont mainFont) : wxPanel(parent, wxID_ANY)
{
	parent_ = parent;
	Create(image);
	Set(headerFont, mainFont);
	CreateSizer();
}
void PanelSender::BindControl(PanelExplorer* panelExplorer, std::string &file_name, std::string &app_svc_name, int &processID, std::string receive_email, wxScopedPtr<GmailSender>& gmailSender)
{
	Features->Bind(wxEVT_RADIOBOX, &PanelSender::OnFeaturesChanged, this);
	OptionsAppSvc->Bind(wxEVT_RADIOBOX, &PanelSender::OnOptionsChanged, this);
	OptionsFile->Bind(wxEVT_RADIOBOX, &PanelSender::OnOptionsChanged, this);
	ButtonExit->Bind(wxEVT_BUTTON, &PanelSender::OnButtonExitClicked, this);
	ButtonConfirm->Bind(wxEVT_BUTTON, [this, panelExplorer, &file_name, &app_svc_name, &processID, receive_email, &gmailSender](wxCommandEvent&) {
		OnButtonConfirmClicked(panelExplorer, file_name, app_svc_name, processID, receive_email, gmailSender);
		});

}
void PanelSender::Create(wxImage image)
{
	Image = image.Scale(120, 120, wxIMAGE_QUALITY_HIGH);
	wxArrayString features = { "Application", "Service", "File", "Screen Capture", "Webcam", "Shutdown" };
	wxArrayString optionsAppSvc = { "List", "Explore", "Start", "Stop" };
	wxArrayString optionsLGD = { "List", "Explore", "Get", "Delete" };
	wxArrayString optionsCamera = { "Open", "Close", "Record", "Capture" };

	TextTitle = new wxStaticText(this, wxID_ANY, "ROLE: SENDER");
	Features = new wxRadioBox(this, wxID_ANY, "Features", wxDefaultPosition, wxDefaultSize, features, 6, wxRA_SPECIFY_ROWS);
	OptionsAppSvc = new wxRadioBox(this, wxID_ANY, "Options", wxDefaultPosition, wxDefaultSize, optionsAppSvc, 2, wxRA_SPECIFY_COLS);
	OptionsFile = new wxRadioBox(this, wxID_ANY, "Options", wxDefaultPosition, wxDefaultSize, optionsLGD, 2, wxRA_SPECIFY_COLS);
	OptionsCamera = new wxRadioBox(this, wxID_ANY, "Options", wxDefaultPosition, wxDefaultSize, optionsCamera, 2, wxRA_SPECIFY_COLS);
	ButtonConfirm = new wxButton(this, wxID_ANY, "Confirm");
	ButtonClose = new wxButton(this, wxID_ANY, "Close");
	ButtonExit = new wxButton(this, wxID_ANY, "Exit");
	ImageDisplay = new wxStaticBitmap(this, wxID_ANY, wxBitmap(Image));
	InputFieldProcessID = new wxTextCtrl(this, wxID_ANY, "Enter process ID", wxDefaultPosition, wxSize(175, -1), 0, wxTextValidator(wxFILTER_NUMERIC));
	InputFieldAppSvcName = new wxTextCtrl(this, wxID_ANY, "Enter application name", wxDefaultPosition, wxSize(175, -1));
	InputFieldFileName = new wxTextCtrl(this, wxID_ANY, "Enter file name", wxDefaultPosition, wxSize(210, -1));

}
void PanelSender::Set(wxFont headerFont, wxFont mainFont)
{
	TextTitle->SetFont(headerFont);
	Features->SetFont(mainFont);
	OptionsAppSvc->SetFont(mainFont);
	OptionsFile->SetFont(mainFont);
	OptionsCamera->SetFont(mainFont);
	OptionsFile->Hide();
	OptionsCamera->Hide();
	InputFieldProcessID->Hide();
	InputFieldAppSvcName->Hide();
	InputFieldFileName->Hide();
	ButtonClose->Hide();
	ButtonConfirm->SetFont(mainFont);
	ButtonClose->SetFont(mainFont);
	ButtonExit->SetFont(mainFont);
	InputFieldFileName->SetFont(mainFont);
	InputFieldAppSvcName->SetFont(mainFont);
	InputFieldProcessID->SetFont(mainFont);
}
void PanelSender::CreateSizer()
{
	MainSizer = new wxBoxSizer(wxVERTICAL);
	SubSizer1 = new wxBoxSizer(wxHORIZONTAL);
	SubSizer2 = new wxBoxSizer(wxHORIZONTAL);
	SubSizer3 = new wxBoxSizer(wxHORIZONTAL);
	SubSizer4 = new wxBoxSizer(wxVERTICAL);

	SubSizer3->Add(ButtonConfirm);
	SubSizer3->AddSpacer(30);
	SubSizer3->Add(ButtonClose);

	SubSizer4->Add(SubSizer3, 0, wxTOP | wxBOTTOM, 10);
	SubSizer4->Add(OptionsAppSvc);
	SubSizer4->Add(OptionsFile);
	SubSizer4->Add(OptionsCamera);
	SubSizer4->AddSpacer(20);
	SubSizer4->Add(InputFieldProcessID);
	SubSizer4->Add(InputFieldAppSvcName);
	SubSizer4->Add(InputFieldFileName);

	SubSizer1->AddSpacer(30);
	SubSizer1->Add(TextTitle, 0, wxTOP, 25);
	SubSizer1->AddSpacer(100);
	SubSizer1->Add(ImageDisplay);

	SubSizer2->AddSpacer(50);
	SubSizer2->Add(Features);
	SubSizer2->AddSpacer(100);
	SubSizer2->Add(SubSizer4);

	MainSizer->Add(SubSizer1);
	MainSizer->Add(SubSizer2);
	MainSizer->Add(ButtonExit, 0, wxALIGN_CENTER_HORIZONTAL | wxTOP, 10);
	this->SetSizer(MainSizer);
}
void PanelSender::OnFeaturesChanged(wxCommandEvent& evt)
{
	int features = evt.GetInt();

	InputFieldFileName->Hide();
	InputFieldAppSvcName->Hide();
	InputFieldProcessID->Hide();

	OptionsAppSvc->SetSelection(0);
	OptionsAppSvc->Hide();
	OptionsFile->SetSelection(0);
	OptionsFile->Hide();
	OptionsCamera->SetSelection(0);
	OptionsCamera->Hide();

	ButtonClose->Hide();

	switch (features) {
	case 0: case 1:
		OptionsAppSvc->Show();
		break;
	case 2:
		OptionsFile->Show();
		break;
	case 4:
		OptionsCamera->Show();
		break;
	}

	this->Layout();
}
void PanelSender::OnOptionsChanged(wxCommandEvent& evt) {
	int features = Features->GetSelection();
	int options = evt.GetInt();

	InputFieldFileName->Hide();
	InputFieldAppSvcName->Hide();
	InputFieldProcessID->Hide();

	if (features == 0) {
		if (options == 2) {
			InputFieldAppSvcName->Show();
			InputFieldAppSvcName->SetValue("Enter Application Name");
		}
		else if (options == 3) {
			InputFieldProcessID->Show();
			InputFieldProcessID->SetValue("Enter Process ID");
		}
	}
	else if (features == 1) {
		if (options == 2) {
			InputFieldAppSvcName->Show();
			InputFieldAppSvcName->SetValue("Enter Service Name");
		}
		else if (options == 3) {
			InputFieldProcessID->Show();
			InputFieldProcessID->SetValue("Enter Process ID");
		}
	}
	else if (features == 2) {
		if (options == 2 || options == 3) {
			InputFieldFileName->Show();
			InputFieldFileName->SetValue("Enter File Name");
		}
	}

	this->Layout();
}
void PanelSender::OnButtonConfirmClicked(PanelExplorer* panelExplorer, std::string &file_name, std::string &app_svc_name, int &processID, std::string receive_email, wxScopedPtr<GmailSender>& gmailSender)
{
	// Get the seletion from features and options
	int featureSelection = Features->GetSelection();
	int optionSelection = -1;
	if (OptionsAppSvc->IsShown()) {
		optionSelection = OptionsAppSvc->GetSelection();
	}
	else if (OptionsFile->IsShown()) {
		optionSelection = OptionsFile->GetSelection();
	}
	else if (OptionsCamera->IsShown()) {
		optionSelection = OptionsCamera->GetSelection();
	}
	file_name = InputFieldFileName->GetValue().ToStdString();
	app_svc_name = InputFieldAppSvcName->GetValue().ToStdString();
	processID = wxAtoi(InputFieldProcessID->GetValue());

	std::string feature = Features->GetString(featureSelection).ToStdString();
	std::string option = "";
	if (OptionsAppSvc->IsShown())
		option = OptionsAppSvc->GetString(optionSelection).ToStdString();
	else if (OptionsFile->IsShown())
		option = OptionsFile->GetString(optionSelection).ToStdString();
	else if (OptionsCamera->IsShown())
		option = OptionsCamera->GetString(optionSelection).ToStdString();
	else option = "";

	// Show the value of vars to check if corrected
	std::cout << "Feature: " << feature << std::endl;
	std::cout << "Option: " << option << std::endl;
	std::cout << "Process ID: " << processID << std::endl;
	std::cout << "File name: " << file_name << std::endl;
	std::cout << std::endl;

	// Send email
	std::string body;
	switch (featureSelection) {
	case 0:
		if (optionSelection == 0) {
			// list app
			body = "list app";
			break;
		}
		else if (optionSelection == 1) {
			// explore app
			panelExplorer->ReconstructTable(0);
			if (panelExplorer->PopulateTableFromFile("apps_list.txt", 0)) {
				this->Hide();
				panelExplorer->Show();
				panelExplorer->Layout();
				parent_->Layout();
			}
			break;
		}
		else if (optionSelection == 2) {
			// start app
			body = "start app " + app_svc_name;
			break;
		}
		else if (optionSelection == 3) {
			// stop app
			body = "kill " + std::to_string(processID);
			break;
		}
	case 1:
		if (optionSelection == 0) {
			// list service
			body = "list service";
			break;
		}
		else if (optionSelection == 1) {
			// explore service
			panelExplorer->ReconstructTable(1);
			if (panelExplorer->PopulateTableFromFile("services_list.txt", 1)) {
				this->Hide();
				panelExplorer->Show();
				panelExplorer->Layout();
				parent_->Layout();
			}
			break;
		}
		else if (optionSelection == 2) {
			// start service
			body = "start service " + app_svc_name;
			break;
		}
		else if (optionSelection == 3) {
			// stop service
			body = "kill " + std::to_string(processID);
			break;
		}
	case 2:
		if (optionSelection == 0) {
			// list file
			body = "list file";
			break;
		}
		else if (optionSelection == 1) {
			// explore file
			panelExplorer->ReconstructTable(2);
			if (panelExplorer->PopulateTableFromFile("files_list.txt", 2)) {
				this->Hide();
				panelExplorer->Show();
				panelExplorer->Layout();
				parent_->Layout();
			}
			break;
		}
		else if (optionSelection == 2) {
			// get file
			body = "get " + file_name;
			break;
		}
		else if (optionSelection == 3) {
			// delete file
			body = "remove " + file_name;
			break;
		}
	case 3:
		// capture screen
		body = "take screenshot";
		break;
	case 4:
		// webcam
		if (optionSelection == 0) {
			// open camera
			body = "start cam";
			break;
		}
		else if (optionSelection == 1) {
			// close camera
			body = "stop cam";
			break;
		}
		else if (optionSelection == 2) {
			// record camera
			body = "record cam";
			break;
		}
		else if (optionSelection == 3) {
			// capture camera
			body = "capture cam";
			break;
		}
	case 5:
		// shut down
		body = "shut down";
		break;
	}

	//gmailSender->setSubject("Remote Control");
	if ((featureSelection == 0 || featureSelection == 1) && optionSelection == 1) return;
	gmailSender->setBody(body);
	std::cout << "Target email: " << gmailSender->m_to << std::endl;
	std::cout << "Subject: " << gmailSender->m_subject << std::endl;
	std::cout << "Body: " << gmailSender->m_body << std::endl;
	std::cout << "Process ID: " << processID << std::endl;
	if (gmailSender->send()) {
		std::cout << "Sent successfully";
	}
	else std::cout << "Error";
}
void PanelSender::OnButtonCloseClicked()
{
	this->parent_->Close();
}
void PanelSender::OnButtonExitClicked(wxCommandEvent& evt)
{
	//Close socket
	parent_->Close(true);
}

PanelReceiver::PanelReceiver(wxWindow* parent, wxImage image, wxFont headerFont, wxFont mainFont, wxStaticText* m_statusText) : wxPanel(parent, wxID_ANY)
{
	parent_ = parent;
	Create(image);
	Set(headerFont, mainFont);
	//CreateSizer(m_statusText);
}
void PanelReceiver::BindControl(wxScopedPtr<Client>& client)
{
	ButtonExit->Bind(wxEVT_BUTTON, &PanelReceiver::OnButtonExitClicked, this);
	//this->Bind(wxEVT_TIMER, &PanelReceiver::OnUpdateTimer, this, UpdateTimer->GetId());
}
void PanelReceiver::Create(wxImage image)
{
	Image = image.Scale(120, 120, wxIMAGE_QUALITY_HIGH);
	TextTitle = new wxStaticText(this, wxID_ANY, "ROLE: RECEIVER");
	ButtonExit = new wxButton(this, wxID_ANY, "Exit");
	ImageDisplay = new wxStaticBitmap(this, wxID_ANY, wxBitmap(Image));

	//TextWaiting = new wxStaticText(this, wxID_ANY, "WAITING FOR COMMAND");
	//TextProcessing = new wxStaticText(this, wxID_ANY, "CURRENT PROCESSING:");
	//TextFeature = new wxStaticText(this, wxID_ANY, "FEATURE");
	//UpdateTimer = new wxTimer(this, wxID_ANY);
	//UpdateTimer->Start(3000);
}
void PanelReceiver::Set(wxFont headerFont, wxFont mainFont)
{
	TextTitle->SetFont(headerFont);
	//TextWaiting->SetFont(headerFont);
	//TextProcessing->SetFont(headerFont);
	//TextFeature->SetFont(headerFont);
	ButtonExit->SetFont(mainFont);
	//TextProcessing->Hide();
	//TextFeature->Hide();
}
void PanelReceiver::CreateSizer(wxStaticText* m_statusText)
{
	MainSizer = new wxBoxSizer(wxVERTICAL);
	SubSizer1 = new wxBoxSizer(wxHORIZONTAL);

	SubSizer1->AddSpacer(30);
	SubSizer1->Add(TextTitle, 0, wxTOP, 25);
	SubSizer1->AddSpacer(100);
	SubSizer1->Add(ImageDisplay);

	MainSizer->Add(SubSizer1);
	MainSizer->Add(m_statusText, 1, wxALIGN_CENTER | wxTOP | wxBOTTOM, 40);
	//MainSizer->Add(TextWaiting, 1, wxALIGN_CENTER | wxTOP | wxBOTTOM, 40);
	//MainSizer->Add(TextProcessing, 1, wxALIGN_CENTER);
	//MainSizer->Add(TextFeature, 1, wxALIGN_CENTER);
	MainSizer->AddSpacer(30);
	MainSizer->Add(ButtonExit, 0, wxALIGN_CENTER | wxBOTTOM, 30);
	this->SetSizer(MainSizer);
}
//bool PanelReceiver::OnEventListened()
//{
//	return rand() % 2 == 0;
//}
void PanelReceiver::OnButtonExitClicked(wxCommandEvent& evt)
{
	//Timer->Stop();
	parent_->Close(true);
}
void PanelReceiver::OnUpdateTimer(wxTimerEvent& event)
{
	this->Layout();
}
//void PanelReceiver::UpdateStatusText()
//{
//	bool commandReceived = OnEventListened();
//	if (commandReceived) {
//		//TextWaiting->Show();
//		//TextProcessing->Hide();
//		//TextFeature->Hide();
//	}
//	else {
//		//TextWaiting->Hide();
//		//TextProcessing->Show();
//		//TextFeature->Show();
//	}
//	this->Layout();
//}

//bool PanelReceiver::OnRun()
//{
//	client->run();
//	return true;
//}


PanelExplorer::PanelExplorer(wxWindow* parent, int& processID) : wxPanel(parent, wxID_ANY)
{
	parent_ = parent;
	format_ = 0;
	Create();
	CreateSizer();
}

void PanelExplorer::BindControl(wxPanel* desPanel, int& processID, std::string& filename, wxScopedPtr<GmailSender>& gmailSender)
{
	ButtonAction->Bind(wxEVT_BUTTON, [this, &processID, &filename, &gmailSender](wxCommandEvent&) {
		OnButtonActionClicked(processID, filename, gmailSender);
		});
	ButtonReturn->Bind(wxEVT_BUTTON, [this, desPanel](wxCommandEvent&) {
		OnButtonReturnClicked(desPanel);
		});

}
void PanelExplorer::Create()
{
	// Create the table (wxListCtrl) in report mode
	table = new wxListCtrl(this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxLC_REPORT | wxLC_SINGLE_SEL);
	//table->SetSize(wxSize(300, 300));
	// Add columns
	table->InsertColumn(0, "PID", wxLIST_FORMAT_LEFT, 100);
	table->InsertColumn(1, "Name", wxLIST_FORMAT_LEFT, 300);

	// Create the action button
	ButtonAction = new wxButton(this, wxID_ANY, "Stop");
	ButtonReturn = new wxButton(this, wxID_ANY, "Return");
}
void PanelExplorer::ReconstructTable(int format)
{
	table->DeleteAllColumns();
	table->DeleteAllItems();
	// format:
	// 0 -> Apps list; 1 -> Service list; 2 -> File list
	format_ = format;
	switch (format) {
	case 0:
		table->InsertColumn(0, "PID", wxLIST_FORMAT_LEFT, 100);
		table->InsertColumn(1, "Application Name", wxLIST_FORMAT_LEFT, 300);
		ButtonAction->SetLabel("Stop");
		break;
	case 1:
		table->InsertColumn(0, "PID", wxLIST_FORMAT_LEFT, 100);
		table->InsertColumn(1, "Service Name", wxLIST_FORMAT_LEFT, 300);
		ButtonAction->SetLabel("Stop");
		break;
	case 2:
		table->InsertColumn(0, "File name", wxLIST_FORMAT_LEFT, 300);
		ButtonAction->SetLabel("Delete");
		break;
	}
}
void PanelExplorer::CreateSizer()
{
	MainSizer = new wxBoxSizer(wxVERTICAL);
	SubSizer = new wxBoxSizer(wxHORIZONTAL);

	SubSizer->Add(ButtonAction, 0, wxALL, 10);
	SubSizer->Add(ButtonReturn, 0, wxALL, 10);

	MainSizer->Add(table, 1, wxEXPAND | wxALL, 10);
	MainSizer->Add(SubSizer);
	this->SetSizer(MainSizer);
}

bool PanelExplorer::PopulateTableFromFile(const std::string& filePath, int format) {
	std::ifstream file(filePath);
	table->DeleteAllItems();

	if (!file.is_open()) {
		wxMessageBox("Failed to open file: " + wxString(filePath), "Error", wxOK | wxICON_ERROR);
		return false;
	}

	std::string line;
	if (format == 0 || format == 1) {
		bool firstRow = true;

		while (std::getline(file, line)) {
			if (firstRow) {
				firstRow = false; // Skip the first row (header)
				continue;
			}

			// Find "PID:" and "Name:"
			size_t pidPos = line.find("PID:");
			size_t namePos = line.find("Name:");

			if (pidPos != std::string::npos && namePos != std::string::npos) {
				// Extract PID and Name
				std::string pid = line.substr(pidPos + 5, namePos - pidPos - 7);
				if (pid == "0") continue;
				std::string name = line.substr(namePos + 5);

				// Insert into wxListCtrl
				long index = table->InsertItem(table->GetItemCount(), pid); // Insert PID
				table->SetItem(index, 1, name);                            // Insert Name
			}
		}
	}
	else if (format == 2) {
		while (std::getline(file, line)) {
			std::string filename = line.substr(2);
			table->InsertItem(table->GetItemCount(), filename);
		}
	}
	file.close();
	return true;
}

void PanelExplorer::OnButtonActionClicked(int& processID, std::string& filename, wxScopedPtr<GmailSender>& gmailSender)
{
	// Get the selected row
	long selectedRow = table->GetNextItem(-1, wxLIST_NEXT_ALL, wxLIST_STATE_SELECTED);

	if (selectedRow == -1) {
		wxMessageBox("No row selected. Please select a row first.", "Error", wxOK | wxICON_WARNING);
		return;
	}

	std::string body;
	wxString tmp = table->GetItemText(selectedRow, 0);
	if (format_ == 0 || format_ == 1) {
		processID = wxAtoi(tmp); // Get PID
		std::cout << "ProcessID in explorer: " << processID << std::endl;
		body = "kill " + std::to_string(processID);;
	}
	else if (format_ == 2) {
		filename = tmp.ToStdString();
		std::cout << "File name: " << filename << std::endl;
		body = "remove " + filename;
	}

	std::cout << "Email body: " << body << std::endl;
	gmailSender->setBody(body);
	//std::cout << "Target email: " << gmailSender->m_to << std::endl;
	std::cout << "Subject: " << gmailSender->m_subject << std::endl;
	std::cout << "Body: " << gmailSender->m_body << std::endl;
	std::cout << "Process ID: " << processID << std::endl;
	if (gmailSender->send()) {
		std::cout << "Sent successfully";
	}
	else std::cout << "Error";
	std::cout << std::endl;

	// Perform your custom action
	//PerformActionOnRow(selectedRow);

	// Delete the selected row
	table->DeleteItem(selectedRow);
}

void PanelExplorer::OnButtonReturnClicked(wxPanel* desPanel)
{
	this->Hide();
	desPanel->Show();
	desPanel->Layout();
	parent_->Layout();
}

//void PanelExplorer::PerformActionOnRow(long selectedRow) {
//	// Retrieve data from the selected row
//	wxString pid = table->GetItemText(selectedRow, 0); // Get PID
//	wxString name = table->GetItemText(selectedRow, 1); // Get Name
//
//	// Perform your custom action here
//	wxMessageBox("Performing action on:\nPID: " + pid + "\nName: " + name, "Info", wxOK | wxICON_INFORMATION);
//}