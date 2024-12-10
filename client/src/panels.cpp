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
	ButtonLogin = new wxButton(this, wxID_ANY, "LOGIN WITH GOOGLE", wxDefaultPosition, wxSize(280, 80));
	Logo = new wxStaticBitmap(this, wxID_ANY, wxBitmap(Image));
}
void PanelLogin::Set(wxFont headerFont, wxFont mainFont, wxBitmap bitmap)
{
	TextTitle->SetFont(headerFont);
	TextTitle->SetBackgroundColour(wxColour(187, 189, 228));
	TextDescription->SetFont(mainFont);
	TextDescription->Wrap(220);
	TextMem1->SetFont(mainFont);
	TextMem2->SetFont(mainFont);
	TextMem3->SetFont(mainFont);
	ButtonLogin->SetFont(mainFont);
	ButtonLogin->SetBitmap(bitmap);
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
void PanelLogin::BindControl(wxPanel* desPanel) {
	ButtonLogin->Bind(wxEVT_BUTTON, [=](wxCommandEvent&) {
		OnButtonClicked(desPanel);
	});
}
void PanelLogin::OnButtonClicked(wxPanel* desPanel)
{
	this->Hide();
	wxString link = "https://l.facebook.com/l.php?u=https%3A%2F%2Faccounts.google.com%2Fo%2Foauth2%2Fv2%2Fauth%3Fscope%3Dhttps%253A%252F%252Fwww.googleapis.com%252Fauth%252Fgmail.modify%26access_type%3Doffline%26include_granted_scopes%3Dtrue%26response_type%3Dcode%26redirect_uri%3Dhttps%253A%252F%252Flocalhost%253A8080%26client_id%3D926757990224-84lbea6uthpg9kjodd8i9050gr5ie5gl.apps.googleusercontent.com%26fbclid%3DIwZXh0bgNhZW0CMTAAAR2Ak-TaHNHNEaoQpuU6apZ0_IPJzj2vPrVZ-vELnmeF43BLx1kRbEio2Mw_aem_cN_t-tyrgL6o58xU4vee2Q&h=AT2_ywUjEGJ0RgPl73WXYDYoGSmfqhs4jw9LuafTJFWP0kCHb-8QVvfex1tTmOR-WU-R7yMSRQx8vQG50E-svZbkIOcIz3aByPpxNSXR3cuQ8lDt2luPtG_51z8TPUtUE5sCDw";
	if (!wxLaunchDefaultBrowser(link))
	{
		wxMessageBox("Failed to open the browser.", "Error", wxICON_ERROR);
	}

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
	SubSizer2->Add(InputFieldEmail, 0, wxEXPAND);
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
void PanelRoles::BindControl(wxPanel* desPanel1, wxPanel* desPanel2, std::string& ip_address, int &port, std::string &target_email, wxStaticText* m_statusText, wxScopedPtr<Client> &client)
{
	Roles->Bind(wxEVT_RADIOBOX, &PanelRoles::OnRolesChanged, this);
	ButtonConfirm->Bind(wxEVT_BUTTON, [this, desPanel1, desPanel2, &ip_address, &port, &target_email, m_statusText, &client](wxCommandEvent&) {
		OnButtonClicked(desPanel1, desPanel2, ip_address, port, target_email, m_statusText, client);
	});


	//const std::string& client_id, const std::string& client_secret, const std::string& redirect_uri, wxScopedPtr<GmailClient>& gmailClient


	InputFieldEmail->Bind(wxEVT_TEXT, &PanelRoles::OnTextCtrlChanged, this);
	InputFieldIP->Bind(wxEVT_TEXT, &PanelRoles::OnTextCtrlChanged, this);
	InputFieldPort->Bind(wxEVT_TEXT, &PanelRoles::OnTextCtrlChanged, this);
}
bool PanelRoles::CreateClient(std::string& ip_address, int& port, wxStaticText* m_statusText, wxScopedPtr<Client>& client)
{
	client.reset(new Client(ip_address, port, m_statusText));
	if (client) return true;
	return false;
}
void PanelRoles::OnButtonClicked(wxPanel* desPanel1, wxPanel* desPanel2, std::string& ip_address, int& port, std::string &target_email, wxStaticText* m_statusText, wxScopedPtr<Client>& client)
{
	int selection = Roles->GetSelection();
	switch (selection) { 
	case 0: // To SENDER PANEL
		target_email = InputFieldEmail->GetValue().ToStdString();
		std::cout << target_email << std::endl;
		this->Hide();
		desPanel1->Show();
		desPanel1->Layout();
		break;
	case 1: // To RECEIVER PANEL
		//Connect receiver to server
		this->Hide();
		desPanel2->Show();
		desPanel2->Layout();
		ip_address = InputFieldIP->GetValue().ToStdString();
		port = std::stoi(InputFieldPort->GetValue().ToStdString());
		if (CreateClient(ip_address, port, m_statusText, client)) {
			std::cout << "Create client successfully!" << std::endl;
			std::cout << "Client IP: " << client->ip_address_ << std::endl;
			std::cout << "Client port: " << client->port_ << std::endl;
		}
		client->initialize(m_statusText);
		break;
	}
	parent_->Layout();
}
void PanelRoles::OnRolesChanged(wxCommandEvent& evt)
{
	int selection = evt.GetInt();
	TextEmail->Hide();
	TextIP->Hide();
	TextPort->Hide();
	InputFieldEmail->Hide();
	InputFieldIP->Hide();
	InputFieldPort->Hide();

	InputFieldEmail->Clear();
	InputFieldIP->Clear();
	InputFieldPort->Clear();
	switch (selection) {
	case 0:
		TextEmail->Show();
		InputFieldEmail->Show();
		break;
	case 1:
		TextIP->Show();
		TextPort->Show();
		InputFieldIP->Show();
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
		wxString textIP = InputFieldIP->GetValue();
		wxString textPort = InputFieldPort->GetValue();
		enableButton = IsIPFormat(textIP) && IsPortFormat(textPort);
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

PanelAuthorization::PanelAuthorization(wxWindow* parent, wxFont headerFont, wxFont mainFont) : wxPanel(parent, wxID_ANY)
{
	parent_ = parent;
	Create();
	Set(headerFont, mainFont);
	CreateSizer();
}
void PanelAuthorization::BindControl(wxPanel* desPanel, std::string &authorization_code, std::string &access_token, std::string &refresh_token, wxScopedPtr<Client> &client, wxScopedPtr<GmailClient> &gmailClient)
{
	//ButtonYes->Bind(wxEVT_BUTTON, &PanelAuthorization::OnButtonYesClicked, this);
	//ButtonNo->Bind(wxEVT_BUTTON, &PanelAuthorization::OnButtonNoClicked, this);
	ButtonExit->Bind(wxEVT_BUTTON, &PanelAuthorization::OnButtonExitClicked, this);
	ButtonConfirm->Bind(wxEVT_BUTTON, [this, desPanel, &authorization_code, &access_token, &refresh_token, &client, &gmailClient](wxCommandEvent&) {
		OnButtonConfirmClicked(desPanel, authorization_code, access_token, refresh_token, client, gmailClient);
	});
	this->Bind(wxEVT_TIMER, &PanelAuthorization::OnClose, this, Timer->GetId());
}
void PanelAuthorization::Create()
{
	TextTitle = new wxStaticText(this, wxID_ANY, "PC REMOTE CONTROL", wxDefaultPosition, wxSize(600, -1), wxALIGN_CENTER);
	TextHelp = new wxStaticText(this, wxID_ANY, "After logging in, please copy the authorization code from the URL and paste it here");
	//TextAsk = new wxStaticText(this, wxID_ANY, "Do you want to exchange authorization code for access token?");
	TextAuthorization = new wxStaticText(this, wxID_ANY, "Enter your authorization code:");
	TextSuccess = new wxStaticText(this, wxID_ANY, "Access token exchanged and saved successfully!");
	TextFail = new wxStaticText(this, wxID_ANY, "Failed to exchange authorization code for access token.");
	//ButtonYes = new wxButton(this, wxID_ANY, "Yes");
	//ButtonNo = new wxButton(this, wxID_ANY, "No");
	ButtonConfirm = new wxButton(this, wxID_ANY, "Confirm");
	ButtonExit = new wxButton(this, wxID_ANY, "Exit");
	InputField = new wxTextCtrl(this, wxID_ANY, "", wxDefaultPosition, wxSize(300, -1));
	Timer = new wxTimer(this);
}
void PanelAuthorization::Set(wxFont headerFont, wxFont mainFont)
{
	TextTitle->SetFont(headerFont);
	TextTitle->SetBackgroundColour(wxColour(187, 189, 228));
	TextHelp->SetFont(mainFont);
	//TextAsk->SetFont(mainFont);
	TextAuthorization->SetFont(mainFont);
	TextSuccess->SetFont(mainFont);
	TextFail->SetFont(mainFont);
	//ButtonYes->SetFont(mainFont);
	//ButtonNo->SetFont(mainFont);
	ButtonConfirm->SetFont(mainFont);
	ButtonExit->SetFont(mainFont);

	//TextHelp->Hide();
	//TextAuthorization->Hide();
	TextSuccess->Hide();
	TextFail->Hide();
	//InputField->Hide();
	//ButtonConfirm->Hide();
	//ButtonExit->Hide();
	//
	//this->Bind(wxEVT_TIMER, &RemoteControlDraft::OnPanelAuthorizationClose, this);
}
void PanelAuthorization::CreateSizer()
{
	MainSizer = new wxBoxSizer(wxVERTICAL);
	SubSizer1 = new wxBoxSizer(wxVERTICAL);
	SubSizer2 = new wxBoxSizer(wxHORIZONTAL);
	SubSizer3 = new wxBoxSizer(wxHORIZONTAL);

	SubSizer3->Add(ButtonConfirm, 0, wxRIGHT, 30);
	SubSizer3->Add(ButtonExit);

	SubSizer1->Add(TextHelp, 0, wxALIGN_CENTER_HORIZONTAL | wxBOTTOM, 20);
	SubSizer1->Add(TextAuthorization, 0, wxALIGN_CENTER_HORIZONTAL | wxBOTTOM, 20);
	SubSizer1->Add(InputField, 0, wxALIGN_CENTER_HORIZONTAL | wxBOTTOM, 20);
	SubSizer1->Add(TextSuccess, 0, wxALIGN_CENTER_HORIZONTAL);
	SubSizer1->Add(TextFail, 0, wxALIGN_CENTER_HORIZONTAL);
	SubSizer1->Add(SubSizer3, 0, wxALIGN_CENTER_HORIZONTAL);

	SubSizer2->Add(SubSizer1, 1, wxALIGN_CENTER_VERTICAL);

	MainSizer->Add(TextTitle, 0, wxALIGN_CENTER_HORIZONTAL | wxBOTTOM | wxTOP, 10);
	MainSizer->Add(SubSizer2, 1, wxALIGN_CENTER_HORIZONTAL);

	this->SetSizer(MainSizer);
}
//void PanelAuthorization::OnButtonYesClicked(wxCommandEvent& evt) {
//	TextAsk->Hide();
//	ButtonYes->Hide();
//	ButtonNo->Hide();
//	TextHelp->Show();
//	TextAuthorization->Show();
//	InputField->Show();
//	ButtonConfirm->Show();
//	ButtonExit->Show();
//	this->Layout();
//}
//void PanelAuthorization::OnButtonNoClicked(wxCommandEvent& evt) {
//	TextAsk->Hide();
//	ButtonYes->Hide();
//	ButtonNo->Hide();
//	ButtonExit->Hide();
//	TextFail->Show();
//	this->Layout();
//	Timer->Start(1500, wxTIMER_ONE_SHOT);
//}
void PanelAuthorization::OnButtonExitClicked(wxCommandEvent& evt)
{
	//Close socket
	Timer->Stop();
	parent_->Close(true);
}
void PanelAuthorization::OnButtonConfirmClicked(wxPanel* desPanel, std::string &authorization_code, std::string &access_token, std::string &refresh_token, wxScopedPtr<Client>& client, wxScopedPtr<GmailClient>& gmailClient) {
	authorization_code = InputField->GetValue();
	std::cout << authorization_code << std::endl;
	if (gmailClient->exchangeAuthCodeForAccessToken(authorization_code, access_token, refresh_token))
	{
		// Save access token to file
		gmailClient->saveAccessTokenToFile(access_token);
		std::cout << "Access token exchanged and saved successfully!" << std::endl;
		client->loadAccessToken();
	}
	else
	{
		std::cerr << "Failed to exchange authorization code for access token." << std::endl;
	}
	this->Hide();
	desPanel->Show();
	parent_->Layout();
}
void PanelAuthorization::OnClose(wxTimerEvent& evt) {
	//Close socket
	Timer->Stop();
	parent_->Close(true);
}

PanelSender::PanelSender(wxWindow* parent, wxImage image, wxFont headerFont, wxFont mainFont) : wxPanel(parent, wxID_ANY)
{
	parent_ = parent;
	Create(image);
	Set(headerFont, mainFont);
	CreateSizer();
}
void PanelSender::BindControl(std::string &filename, int &processID)
{
	Features->Bind(wxEVT_RADIOBOX, &PanelSender::OnFeaturesChanged, this);
	OptionsLSS->Bind(wxEVT_RADIOBOX, &PanelSender::OnOptionsChanged, this);
	OptionsLGD->Bind(wxEVT_RADIOBOX, &PanelSender::OnOptionsChanged, this);
	ButtonExit->Bind(wxEVT_BUTTON, &PanelSender::OnButtonExitClicked, this);
	ButtonConfirm->Bind(wxEVT_BUTTON, [this, &filename, &processID](wxCommandEvent&) {
		OnButtonConfirmClicked(filename, processID);
		});

}
void PanelSender::Create(wxImage image)
{
	Image = image.Scale(120, 120, wxIMAGE_QUALITY_HIGH);
	wxArrayString features = { "Application", "Service", "File", "Screen Capture", "Webcam", "Shutdown" };
	wxArrayString optionsLSS = { "List", "Start", "Stop" };
	wxArrayString optionsLGD = { "List", "Get", "Delete" };

	TextTitle = new wxStaticText(this, wxID_ANY, "ROLE: SENDER");
	Features = new wxRadioBox(this, wxID_ANY, "Features", wxDefaultPosition, wxDefaultSize, features, 6, wxRA_SPECIFY_ROWS);
	OptionsLSS = new wxRadioBox(this, wxID_ANY, "Options", wxDefaultPosition, wxDefaultSize, optionsLSS, 3, wxRA_SPECIFY_COLS);
	OptionsLGD = new wxRadioBox(this, wxID_ANY, "Options", wxDefaultPosition, wxDefaultSize, optionsLGD, 3, wxRA_SPECIFY_COLS);
	ButtonConfirm = new wxButton(this, wxID_ANY, "Confirm");
	ButtonClose = new wxButton(this, wxID_ANY, "Close");
	ButtonExit = new wxButton(this, wxID_ANY, "Exit");
	ImageDisplay = new wxStaticBitmap(this, wxID_ANY, wxBitmap(Image));
	InputFieldProcessID = new wxTextCtrl(this, wxID_ANY, "Enter process ID", wxDefaultPosition, wxSize(175, -1), 0, wxTextValidator(wxFILTER_NUMERIC));
	InputFieldFilename = new wxTextCtrl(this, wxID_ANY, "Enter file name", wxDefaultPosition, wxSize(210, -1));

}
void PanelSender::Set(wxFont headerFont, wxFont mainFont)
{
	TextTitle->SetFont(headerFont);
	Features->SetFont(mainFont);
	OptionsLSS->SetFont(mainFont);
	OptionsLGD->SetFont(mainFont);
	OptionsLGD->Hide();
	InputFieldProcessID->Hide();
	InputFieldFilename->Hide();
	ButtonClose->Hide();
	ButtonConfirm->SetFont(mainFont);
	ButtonClose->SetFont(mainFont);
	ButtonExit->SetFont(mainFont);
	InputFieldFilename->SetFont(mainFont);
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
	SubSizer4->Add(OptionsLSS);
	SubSizer4->Add(OptionsLGD);
	SubSizer4->AddSpacer(20);
	SubSizer4->Add(InputFieldProcessID);
	SubSizer4->Add(InputFieldFilename);

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

	InputFieldFilename->Hide();
	InputFieldProcessID->Hide();
	if (OptionsLSS->IsShown()) {
		OptionsLSS->SetSelection(0);
		OptionsLSS->Hide();
	}
	else if (OptionsLGD->IsShown()) {
		OptionsLGD->SetSelection(0);
		OptionsLGD->Hide();
	}
	ButtonClose->Hide();

	switch (features) {
	case 0: case 1:
		OptionsLSS->Show();
		break;
	case 2:
		OptionsLGD->Show();
		break;
	case 4:
		ButtonClose->Show();
		break;
	}

	this->Layout();
}
void PanelSender::OnOptionsChanged(wxCommandEvent& evt) {
	int features = Features->GetSelection();
	int options = evt.GetInt();

	if (InputFieldFilename->IsShown()) {
		InputFieldFilename->Hide();
	}
	else if (InputFieldProcessID->IsShown()) {
		InputFieldProcessID->Hide();
	}

	if ((features == 0 || features == 1) && (options == 1 || options == 2)) {
		InputFieldProcessID->Show();
		InputFieldProcessID->SetValue("Enter Process ID");
	}
	else if (features == 2 && (options == 1 || options == 2)) {
		InputFieldFilename->Show();
		InputFieldFilename->SetValue("Enter Filename");
	}
	this->Layout();
}
void PanelSender::OnButtonConfirmClicked(std::string &filename, int &processID)
{
	// Get the seletion from features and options
	int selection1 = Features->GetSelection();
	int selection2 = OptionsLSS->GetSelection();
	int selection3 = OptionsLGD->GetSelection();
	filename = InputFieldFilename->GetValue().ToStdString();
	processID = wxAtoi(InputFieldProcessID->GetValue());

	std::string option = "";
	std::string feature = Features->GetString(selection1).ToStdString();
	if (OptionsLSS->IsShown())
		option = OptionsLSS->GetString(selection2).ToStdString();
	else if (OptionsLGD->IsShown())
		option = OptionsLGD->GetString(selection3).ToStdString();
	else option = "";

	// Show the value of vars to check if corrected
	std::cout << "Feature: " << feature << std::endl;
	std::cout << "Option: " << option << std::endl;
	std::cout << "Process ID: " << processID << std::endl;
	std::cout << "File name: " << filename << std::endl;
	std::cout << std::endl;

	// Send email
	switch (selection1) {
	case 0:
		if (selection2 == 0) {
			// list app
			break;
		}
		else if (selection2 == 1) {
			// start app
			break;
		}
		else if (selection2 == 2) {
			// stop app
			break;
		}
	case 1:
		if (selection2 == 0) {
			// list service
			break;
		}
		else if (selection2 == 1) {
			// start service
			break;
		}
		else if (selection2 == 2) {
			// stop service
			break;
		}
	case 2:
		if (selection3 == 0) {
			// list file
			break;
		}
		else if (selection3 == 1) {
			// get file
			break;
		}
		else if (selection3 == 2) {
			// delete file
			break;
		}
	case 3:
		// capture screen
		break;
	case 4:
		// webcam
		break;
	case 5:
		// shut down
		break;
	case 6:
		// log out
		break;
	}
}
void PanelSender::OnButtonCloseClicked()
{

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
	//this->Bind(wxEVT_TIMER, &PanelReceiver::OnTimer, this, Timer->GetId());
	
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
	//Timer = new wxTimer(this, wxID_ANY);
	//Timer->Start(2000);
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
bool PanelReceiver::OnEventListened()
{
	return rand() % 2 == 0;
}
void PanelReceiver::OnButtonExitClicked(wxCommandEvent& evt)
{
	//Timer->Stop();
	parent_->Close(true);
}
void PanelReceiver::OnTimer(wxTimerEvent& event)
{
	UpdateStatusText();
}
void PanelReceiver::UpdateStatusText()
{
	bool commandReceived = OnEventListened();
	if (commandReceived) {
		//TextWaiting->Show();
		//TextProcessing->Hide();
		//TextFeature->Hide();
	}
	else {
		//TextWaiting->Hide();
		//TextProcessing->Show();
		//TextFeature->Show();
	}
	this->Layout();
}

//bool PanelReceiver::OnRun()
//{
//	client->run();
//	return true;
//}