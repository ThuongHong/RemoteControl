#include "RemoteControlDraft.h"
#include <wx/wx.h>
#include <wx/webview.h>
#include <regex>
#include <windows.h>
#include "panel_login.h"

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
	roles = { "Sender", "Receiver" };

	/*Create Panel*/
	PanelLogin* panelLogin = new PanelLogin(this, description, image, headerFont, mainFont, bitmap);
	//panelLogin = new wxPanel(this);
	//panelLogin->Hide();
	panel2 = new wxPanel(this);
	panel2->Hide();
	panelSender = new wxPanel(this);
	panelSender->Hide();
	panelAuthorization = new wxPanel(this);
	panelAuthorization->Hide();
	panelReceiver = new wxPanel(this);
	panelReceiver->Hide();

	// wxWebView* webView = wxWebView::New(this, wxID_ANY, "https://mail.google.com/", wxDefaultPosition, wxSize(800, 600));

	/*PANEL 1*/
	//CreatePanelLogin(description, image);
	//SetPanelLogin(headerFont, mainFont, bitmap);
	//CreateSizerPanelLogin();

	/*PANEL 2*/
	CreatePanel2(description, image);
	SetPanel2(headerFont, mainFont);
	CreateSizerPanel2();

	panelLogin->BindControl(panel2);
	this->Layout();

	/*PANEL 3*/
	CreatePanelSender(image);
	SetPanelSender(headerFont, mainFont);
	CreateSizerPanelSender();

	/*PANEL 4*/
	CreatePanelAuthorization();
	SetPanelAuthorization(headerFont, mainFont);
	CreateSizerPanelAuthorization();

	/*PANEL 5*/
	CreatePanelReceiver();
	SetPanelReceiver(headerFont, mainFont);
	CreateSizerPanelReceiver();

	BindControl();
	/**/
	sizerMain = new wxBoxSizer(wxVERTICAL);
	sizerMain->Add(panelLogin, 1, wxEXPAND);
	sizerMain->Add(panel2, 1, wxEXPAND);
	sizerMain->Add(panelAuthorization, 1, wxEXPAND);
	sizerMain->Add(panelSender, 1, wxEXPAND);
	sizerMain->Add(panelReceiver, 1, wxEXPAND);

	this->SetSizer(sizerMain);
}

void RemoteControlDraft::CreatePanelLogin(wxString description, wxImage image) {
	panelLoginImage = image.Scale(180, 180, wxIMAGE_QUALITY_HIGH);
	panelLoginTextTitle = new wxStaticText(panelLogin, wxID_ANY, "PC REMOTE CONTROL", wxDefaultPosition, wxSize(600, -1), wxALIGN_CENTER);
	panelLoginTextDescription = new wxStaticText(panelLogin, wxID_ANY, description);
	/*wxStaticText* panelLoginTextMembers = new wxStaticText(panelLogin, wxID_ANY, "Members");
	wxStaticText* panelLoginTextMem1 = new wxStaticText(panelLogin, wxID_ANY, "Chung Tin Dat - 23122024");
	wxStaticText* panelLoginTextMem2 = new wxStaticText(panelLogin, wxID_ANY, "Luu Thuong Hong - 23122006");
	wxStaticText* panelLoginTextMem3 = new wxStaticText(panelLogin, wxID_ANY, "Nguyen Tan Hung - 231220  ");*/
	panelLoginButtonLogin = new wxButton(panelLogin, wxID_ANY, "LOGIN WITH GOOGLE", wxDefaultPosition, wxSize(280, 80));
	panelLoginLogo = new wxStaticBitmap(panelLogin, wxID_ANY, wxBitmap(panelLoginImage));
}
void RemoteControlDraft::SetPanelLogin(wxFont headerFont, wxFont mainFont, wxBitmap bitmap)
{
	panelLoginTextTitle->SetFont(headerFont);
	panelLoginTextTitle->SetBackgroundColour(wxColour(255, 228, 196));
	panelLoginTextDescription->SetFont(mainFont);
	panelLoginTextDescription->Wrap(500);
	panelLoginButtonLogin->SetFont(mainFont);
	panelLoginButtonLogin->SetBitmap(bitmap);
	panelLoginButtonLogin->SetBitmapPosition(wxLEFT);
	panelLoginButtonLogin->SetBitmapMargins(20, 0);
}
void RemoteControlDraft::CreateSizerPanelLogin()
{
	panelLoginMainSizer = new wxBoxSizer(wxVERTICAL);
	panelLoginSubSizer1 = new wxBoxSizer(wxHORIZONTAL);
	panelLoginSubSizer2 = new wxBoxSizer(wxHORIZONTAL);

	panelLoginSubSizer1->Add(panelLoginTextDescription, 0, wxLEFT, 20);

	panelLoginSubSizer2->Add(panelLoginLogo, 0, wxLEFT, 20);
	panelLoginSubSizer2->Add(panelLoginButtonLogin, 0, wxALIGN_CENTER_VERTICAL | wxLEFT, 35);

	panelLoginMainSizer->Add(panelLoginTextTitle, 0, wxBOTTOM, 10);
	panelLoginMainSizer->Add(panelLoginSubSizer1);
	panelLoginMainSizer->Add(panelLoginSubSizer2);

	panelLogin->SetSizer(panelLoginMainSizer);
}

void RemoteControlDraft::CreatePanel2(wxString description, wxImage image)
{
	panel2TextTitle = new wxStaticText(panel2, wxID_ANY, "PC REMOTE CONTROL", wxDefaultPosition, wxSize(600, -1), wxALIGN_CENTER);
	panel2TextDescription = new wxStaticText(panel2, wxID_ANY, description);
	panel2SubTextEmail = new wxStaticText(panel2, wxID_ANY, "Receiver Email");
	panel2SubTextIP = new wxStaticText(panel2, wxID_ANY, "Server IP");
	panel2SubTextPort = new wxStaticText(panel2, wxID_ANY, "Port");
	panel2Roles = new wxRadioBox(panel2, wxID_ANY, "Choose your role:", wxDefaultPosition, wxDefaultSize, roles, 2, wxRA_SPECIFY_ROWS);
	panel2InputFieldEmail = new wxTextCtrl(panel2, wxID_ANY, "", wxDefaultPosition, wxSize(200, -1));
	panel2InputFieldIP = new wxTextCtrl(panel2, wxID_ANY, "", wxDefaultPosition, wxSize(200, -1));
	panel2InputFieldPort = new wxTextCtrl(panel2, wxID_ANY, "", wxDefaultPosition, wxSize(200, -1));
	panel2ButtonConfirm = new wxButton(panel2, wxID_ANY, "CONFIRM", wxDefaultPosition, wxSize(150, -1));
}
void RemoteControlDraft::SetPanel2(wxFont headerFont, wxFont mainFont)
{
	panel2TextTitle->SetFont(headerFont);
	panel2TextTitle->SetBackgroundColour(wxColour(255, 228, 196));
	panel2TextDescription->SetFont(mainFont);
	panel2TextDescription->Wrap(300);
	panel2Roles->SetFont(mainFont);
	panel2SubTextEmail->SetFont(mainFont);
	panel2SubTextIP->SetFont(mainFont);
	panel2SubTextPort->SetFont(mainFont);
	panel2SubTextIP->Hide();
	panel2SubTextPort->Hide();
	panel2InputFieldIP->Hide();
	panel2InputFieldPort->Hide();
	panel2ButtonConfirm->SetFont(mainFont);
	panel2ButtonConfirm->Disable();
}
void RemoteControlDraft::CreateSizerPanel2()
{
	panel2MainSizer = new wxBoxSizer(wxVERTICAL);
	panel2SubSizer1 = new wxBoxSizer(wxHORIZONTAL);
	panel2SubSizer2 = new wxBoxSizer(wxVERTICAL);

	panel2SubSizer2->Add(panel2SubTextEmail, 0, wxALIGN_CENTER_HORIZONTAL | wxBOTTOM, 5);
	panel2SubSizer2->Add(panel2InputFieldEmail, 0, wxEXPAND);
	panel2SubSizer2->Add(panel2SubTextIP, 0, wxALIGN_CENTER_HORIZONTAL | wxBOTTOM, 5);
	panel2SubSizer2->Add(panel2InputFieldIP, 0, wxEXPAND | wxBOTTOM, 5);
	panel2SubSizer2->Add(panel2SubTextPort, 0, wxALIGN_CENTER_HORIZONTAL | wxBOTTOM, 5);
	panel2SubSizer2->Add(panel2InputFieldPort, 0, wxEXPAND);

	panel2SubSizer1->Add(panel2Roles, 0, wxALIGN_CENTER_VERTICAL | wxLEFT, 80);
	panel2SubSizer1->AddSpacer(80);
	panel2SubSizer1->Add(panel2SubSizer2, 0, wxALIGN_CENTER_VERTICAL);

	panel2MainSizer->Add(panel2TextTitle, 0, wxALIGN_CENTER_HORIZONTAL | wxBOTTOM, 10);
	panel2MainSizer->Add(panel2TextDescription, 0, wxLEFT, 20);
	panel2MainSizer->AddSpacer(20);
	panel2MainSizer->Add(panel2SubSizer1);
	panel2MainSizer->Add(panel2ButtonConfirm, 0, wxALIGN_CENTER_HORIZONTAL | wxTOP, 20);

	panel2->SetSizer(panel2MainSizer);
}

void RemoteControlDraft::CreatePanelSender(wxImage image)
{
	panelSenderImage = image.Scale(120, 120, wxIMAGE_QUALITY_HIGH);
	features = { "Application", "Service", "File", "Screen Capture", "Webcam", "Shutdown" };
	optionsLSS = { "List", "Start", "Stop" };
	optionsTD = { "Take", "Delete" };

	panelSenderTextTitle = new wxStaticText(panelSender, wxID_ANY, "ROLE: SENDER");
	panelSenderFeatures = new wxRadioBox(panelSender, wxID_ANY, "Features", wxDefaultPosition, wxDefaultSize, features, 7, wxRA_SPECIFY_ROWS);
	panelSenderOptionsLSS = new wxRadioBox(panelSender, wxID_ANY, "Options", wxDefaultPosition, wxDefaultSize, optionsLSS, 3, wxRA_SPECIFY_COLS);
	panelSenderOptionsTD = new wxRadioBox(panelSender, wxID_ANY, "Options", wxDefaultPosition, wxDefaultSize, optionsTD, 2, wxRA_SPECIFY_COLS);
	panelSenderButtonConfirm = new wxButton(panelSender, wxID_ANY, "Confirm");
	panelSenderButtonExit = new wxButton(panelSender, wxID_ANY, "Exit");
	panelSenderImageDisplay = new wxStaticBitmap(panelSender, wxID_ANY, wxBitmap(panelSenderImage));
}
void RemoteControlDraft::SetPanelSender(wxFont headerFont, wxFont mainFont) {
	panelSenderTextTitle->SetFont(headerFont);
	panelSenderFeatures->SetFont(mainFont);
	panelSenderOptionsLSS->SetFont(mainFont);
	panelSenderOptionsTD->SetFont(mainFont);
	panelSenderOptionsTD->Hide();
	panelSenderButtonConfirm->SetFont(mainFont);
	panelSenderButtonExit->SetFont(mainFont);
}
void RemoteControlDraft::CreateSizerPanelSender()
{
	panelSenderMainSizer = new wxBoxSizer(wxVERTICAL);
	panelSenderSubSizer1 = new wxBoxSizer(wxHORIZONTAL);
	panelSenderSubSizer2 = new wxBoxSizer(wxHORIZONTAL);
	panelSenderSubSizer3 = new wxBoxSizer(wxHORIZONTAL);
	panelSenderSubSizer4 = new wxBoxSizer(wxVERTICAL);

	panelSenderSubSizer3->Add(panelSenderButtonConfirm);
	panelSenderSubSizer3->AddSpacer(30);
	panelSenderSubSizer3->Add(panelSenderButtonExit);

	panelSenderSubSizer4->Add(panelSenderOptionsLSS);
	panelSenderSubSizer4->Add(panelSenderOptionsTD);
	panelSenderSubSizer4->AddSpacer(30);
	panelSenderSubSizer4->Add(panelSenderSubSizer3);

	panelSenderSubSizer1->AddSpacer(30);
	panelSenderSubSizer1->Add(panelSenderTextTitle, 0, wxTOP, 25);
	panelSenderSubSizer1->AddSpacer(100);
	panelSenderSubSizer1->Add(panelSenderImageDisplay);

	panelSenderSubSizer2->AddSpacer(50);
	panelSenderSubSizer2->Add(panelSenderFeatures);
	panelSenderSubSizer2->AddSpacer(100);
	panelSenderSubSizer2->Add(panelSenderSubSizer4, 0, wxALIGN_CENTER);

	panelSenderMainSizer->Add(panelSenderSubSizer1);
	panelSenderMainSizer->Add(panelSenderSubSizer2);
	panelSender->SetSizer(panelSenderMainSizer);
}

void RemoteControlDraft::CreatePanelAuthorization()
{
	panelAuthorizationTextTitle = new wxStaticText(panelAuthorization, wxID_ANY, "PC REMOTE CONTROL", wxDefaultPosition, wxSize(600, -1), wxALIGN_CENTER);
	panelAuthorizationTextDescription = new wxStaticText(panelAuthorization, wxID_ANY, description);

	panelAuthorizationTextAsk = new wxStaticText(panelAuthorization, wxID_ANY, "Do you want to exchange authorization code for access token?");
	panelAuthorizationTextAuthorization = new wxStaticText(panelAuthorization, wxID_ANY, "Enter your authorization code:");
	panelAuthorizationTextSuccess = new wxStaticText(panelAuthorization, wxID_ANY, "Access token exchanged and saved successfully!");
	panelAuthorizationTextFail = new wxStaticText(panelAuthorization, wxID_ANY, "Failed to exchange authorization code for access token.");
	panelAuthorizationButtonYes = new wxButton(panelAuthorization, wxID_ANY, "Yes");
	panelAuthorizationButtonNo = new wxButton(panelAuthorization, wxID_ANY, "No");
	panelAuthorizationButtonConfirm = new wxButton(panelAuthorization, wxID_ANY, "Confirm");
	panelAuthorizationButtonExit = new wxButton(panelAuthorization, wxID_ANY, "Exit");
	panelAuthorizationInputField = new wxTextCtrl(panelAuthorization, wxID_ANY, "", wxDefaultPosition, wxSize(300, -1));
	panelAuthorizationTimer = new wxTimer(panelAuthorization);
}
void RemoteControlDraft::SetPanelAuthorization(wxFont headerFont, wxFont mainFont)
{
	panelAuthorizationTextTitle->SetFont(headerFont);
	panelAuthorizationTextTitle->SetBackgroundColour(wxColour(255, 228, 196));
	panelAuthorizationTextDescription->SetFont(mainFont);
	panelAuthorizationTextDescription->Wrap(300);
	panelAuthorizationTextAsk->SetFont(mainFont);
	panelAuthorizationTextAuthorization->SetFont(mainFont);
	panelAuthorizationTextSuccess->SetFont(mainFont);
	panelAuthorizationTextFail->SetFont(mainFont);
	panelAuthorizationButtonYes->SetFont(mainFont);
	panelAuthorizationButtonNo->SetFont(mainFont);
	panelAuthorizationButtonConfirm->SetFont(mainFont);
	panelAuthorizationButtonExit->SetFont(mainFont);

	panelAuthorizationTextAuthorization->Hide();
	panelAuthorizationTextSuccess->Hide();
	panelAuthorizationTextFail->Hide();
	panelAuthorizationInputField->Hide();
	panelAuthorizationButtonConfirm->Hide();
	panelAuthorizationButtonExit->Hide();
	panelAuthorization->Bind(wxEVT_TIMER, &RemoteControlDraft::OnPanelAuthorizationClose, this);
}
void RemoteControlDraft::CreateSizerPanelAuthorization()
{
	panelAuthorizationMainSizer = new wxBoxSizer(wxVERTICAL);
	panelAuthorizationSubSizer1 = new wxBoxSizer(wxHORIZONTAL);
	panelAuthorizationSubSizer2 = new wxBoxSizer(wxVERTICAL);
	panelAuthorizationSubSizer3 = new wxBoxSizer(wxHORIZONTAL);

	panelAuthorizationSubSizer1->Add(panelAuthorizationButtonYes, 0, wxRIGHT, 30);
	panelAuthorizationSubSizer1->Add(panelAuthorizationButtonNo);
	
	panelAuthorizationSubSizer2->Add(panelAuthorizationTextAsk, 0, wxALIGN_CENTER_HORIZONTAL | wxBOTTOM, 20);
	panelAuthorizationSubSizer2->Add(panelAuthorizationSubSizer1, 0, wxALIGN_CENTER_HORIZONTAL);
	panelAuthorizationSubSizer2->Add(panelAuthorizationTextAuthorization, 0, wxALIGN_CENTER_HORIZONTAL | wxBOTTOM, 20);
	panelAuthorizationSubSizer2->Add(panelAuthorizationInputField, 0, wxALIGN_CENTER_HORIZONTAL | wxBOTTOM, 20);
	panelAuthorizationSubSizer2->Add(panelAuthorizationTextSuccess, 0, wxALIGN_CENTER_HORIZONTAL);
	panelAuthorizationSubSizer2->Add(panelAuthorizationTextFail, 0, wxALIGN_CENTER_HORIZONTAL);
	panelAuthorizationSubSizer2->Add(panelAuthorizationButtonConfirm, 0, wxALIGN_CENTER_HORIZONTAL);
	panelAuthorizationSubSizer2->Add(panelAuthorizationButtonExit, 0, wxALIGN_CENTER_HORIZONTAL);

	panelAuthorizationSubSizer3->Add(panelAuthorizationSubSizer2, 1, wxALIGN_CENTER_VERTICAL);

	panelAuthorizationMainSizer->Add(panelAuthorizationTextTitle, 0, wxALIGN_CENTER_HORIZONTAL | wxBOTTOM, 10);
	panelAuthorizationMainSizer->Add(panelAuthorizationTextDescription, 0, wxLEFT, 20);
	panelAuthorizationMainSizer->Add(panelAuthorizationSubSizer3, 1, wxALIGN_CENTER_HORIZONTAL);

	panelAuthorization->SetSizer(panelAuthorizationMainSizer);
}

void RemoteControlDraft::CreatePanelReceiver()
{
	panelReceiverTextTitle = new wxStaticText(panelReceiver, wxID_ANY, "ROLE: RECEIVER");
	panelReceiverButtonExit = new wxButton(panelReceiver, wxID_ANY, "Exit");
	panelReceiverImageDisplay = new wxStaticBitmap(panelReceiver, wxID_ANY, wxBitmap(panelSenderImage));
	panelReceiverTextWaiting = new wxStaticText(panelReceiver, wxID_ANY, "WAITING FOR COMMAND");
	panelReceiverTextProcessing = new wxStaticText(panelReceiver, wxID_ANY, "CURRENT PROCESSING:");
	panelReceiverTextFeature = new wxStaticText(panelReceiver, wxID_ANY, "FEATURE");
	panelReceiverTimer = new wxTimer(panelReceiver, wxID_ANY);
	panelReceiverTimer->Start(2000);
}
void RemoteControlDraft::SetPanelReceiver(wxFont headerFont, wxFont mainFont)
{
	panelReceiverTextTitle->SetFont(headerFont);
	panelReceiverTextWaiting->SetFont(headerFont);
	panelReceiverTextProcessing->SetFont(headerFont);
	panelReceiverTextFeature->SetFont(headerFont);
	panelReceiverButtonExit->SetFont(mainFont);
	panelReceiverTextProcessing->Hide();
	panelReceiverTextFeature->Hide();
}
void RemoteControlDraft::CreateSizerPanelReceiver()
{
	panelReceiverMainSizer = new wxBoxSizer(wxVERTICAL);
	panelReceiverSubSizer1 = new wxBoxSizer(wxHORIZONTAL);

	panelReceiverSubSizer1->AddSpacer(30);
	panelReceiverSubSizer1->Add(panelReceiverTextTitle, 0, wxTOP, 25);
	panelReceiverSubSizer1->AddSpacer(100);
	panelReceiverSubSizer1->Add(panelReceiverImageDisplay);

	panelReceiverMainSizer->Add(panelReceiverSubSizer1);
	panelReceiverMainSizer->Add(panelReceiverTextWaiting, 1, wxALIGN_CENTER | wxTOP | wxBOTTOM, 40);
	panelReceiverMainSizer->Add(panelReceiverTextProcessing, 1, wxALIGN_CENTER);
	panelReceiverMainSizer->Add(panelReceiverTextFeature, 1, wxALIGN_CENTER);
	panelReceiverMainSizer->AddSpacer(30);
	panelReceiverMainSizer->Add(panelReceiverButtonExit, 0, wxALIGN_CENTER | wxBOTTOM, 30);
	panelReceiver->SetSizer(panelReceiverMainSizer);
}

void RemoteControlDraft::BindControl()
{
	//panelLoginButtonLogin->Bind(wxEVT_BUTTON, &RemoteControlDraft::OnPanelLoginButtonClicked, this);

	panel2Roles->Bind(wxEVT_RADIOBOX, &RemoteControlDraft::OnPanel2RolesChanged, this);
	panel2ButtonConfirm->Bind(wxEVT_BUTTON, &RemoteControlDraft::OnPanel2ButtonClicked, this);
	panel2InputFieldEmail->Bind(wxEVT_TEXT, &RemoteControlDraft::OnPanel2TextCtrlChanged, this);
	panel2InputFieldIP->Bind(wxEVT_TEXT, &RemoteControlDraft::OnPanel2TextCtrlChanged, this);
	panel2InputFieldPort->Bind(wxEVT_TEXT, &RemoteControlDraft::OnPanel2TextCtrlChanged, this);

	panelSenderFeatures->Bind(wxEVT_RADIOBOX, &RemoteControlDraft::OnPanelSenderFeaturesChanged, this);
	panelSenderButtonExit->Bind(wxEVT_BUTTON, &RemoteControlDraft::OnButtonExitClicked, this);
	panelSenderButtonConfirm->Bind(wxEVT_BUTTON, &RemoteControlDraft::OnPanelSenderButtonConfirm, this);

	panelAuthorizationButtonYes->Bind(wxEVT_BUTTON, &RemoteControlDraft::OnPanelAuthorizationButtonYesClicked, this);
	panelAuthorizationButtonNo->Bind(wxEVT_BUTTON, &RemoteControlDraft::OnPanelAuthorizationButtonNoClicked, this);
	panelAuthorizationButtonExit->Bind(wxEVT_BUTTON, &RemoteControlDraft::OnButtonExitClicked, this);
	panelAuthorizationButtonConfirm->Bind(wxEVT_BUTTON, &RemoteControlDraft::OnPanelAuthorizationButtonConfirmClicked, this);

	panelReceiverButtonExit->Bind(wxEVT_BUTTON, &RemoteControlDraft::OnButtonExitClicked, this);
	panelReceiver->Bind(wxEVT_TIMER, &RemoteControlDraft::OnTimer, this, panelReceiverTimer->GetId());
}

void RemoteControlDraft::OnPanelLoginButtonClicked(wxCommandEvent& evt)
{
	//Open webview to sign in gmail
	panelLogin->Hide();
	panel2->Show();
	Layout();
}
void RemoteControlDraft::OnPanel2ButtonClicked(wxCommandEvent& evt)
{
	int selection = panel2Roles->GetSelection();
	switch (selection) {
	case 0: // To SENDER PANEL
		//Set target email
		panel2->Hide();
		panelAuthorization->Show();
		break;
	case 1: // To RECEIVER PANEL
		//Connect receiver to server
		panel2->Hide();
		panelReceiver->Show();
		ip_address = panel2InputFieldIP->GetValue();
		port = std::stoi(panel2InputFieldPort->GetValue().ToStdString());
		if (CreateClient(ip_address, port)) {
			std::cout << "Create client" << std::endl;
			std::cout << "Client IP: " << client->ip_address_ << std::endl;
			std::cout << "Client port: " << client->port_ << std::endl;
		}
		break;
	}
	Layout();
	//panelReceiver->Layout();
	//panelAuthorization->Refresh();
}
void RemoteControlDraft::OnButtonExitClicked(wxCommandEvent& evt)
{
	//Close socket
	Close(true);
	panelReceiverTimer->Stop();
}

void RemoteControlDraft::OnPanelSenderButtonConfirm(wxCommandEvent& evt)
{
	int selection1 = panelSenderFeatures->GetSelection();
	int selection2 = panelSenderOptionsLSS->GetSelection();
	int selection3 = panelSenderOptionsTD->GetSelection();

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
			// take file
			break;
		}
		else if (selection3 == 1) {
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

bool RemoteControlDraft::IsEmailFormat(const wxString& text)
{
	const std::regex emailPattern(R"((\w+)(\.\w+)*@gmail\.com)");
	return std::regex_match(text.ToStdString(), emailPattern);
}
bool RemoteControlDraft::IsIPFormat(const wxString& text) {
	const std::regex ipPattern("^((25[0-5]|2[0-4][0-9]|[01]?[0-9][0-9]?)\\.){3}(25[0-5]|2[0-4][0-9]|[01]?[0-9][0-9]?)$");
	return std::regex_match(text.ToStdString(), ipPattern);
}
bool RemoteControlDraft::IsPortFormat(const wxString& text) {
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

void RemoteControlDraft::OnPanel2RolesChanged(wxCommandEvent& evt)
{
	int selection = evt.GetInt();
	panel2SubTextEmail->Hide();
	panel2SubTextIP->Hide();
	panel2SubTextPort->Hide();
	panel2InputFieldEmail->Hide();
	panel2InputFieldIP->Hide();
	panel2InputFieldPort->Hide();

	panel2InputFieldEmail->Clear();
	panel2InputFieldIP->Clear();
	panel2InputFieldPort->Clear();
	switch (selection) {
	case 0:
		panel2SubTextEmail->Show();
		panel2InputFieldEmail->Show();
		break;
	case 1:
		panel2SubTextIP->Show();
		panel2SubTextPort->Show();
		panel2InputFieldIP->Show();
		panel2InputFieldPort->Show();
		break;
	}
	panel2->Layout();
}
void RemoteControlDraft::OnPanel2TextCtrlChanged(wxCommandEvent& event) {
	bool enableButton = false;

	if (panel2Roles->GetSelection() == 0) {
		wxString textEmail = panel2InputFieldEmail->GetValue();
		enableButton = IsEmailFormat(textEmail);
	}
	else if (panel2Roles->GetSelection() == 1) {
		wxString textIP = panel2InputFieldIP->GetValue();
		wxString textPort = panel2InputFieldPort->GetValue();
		enableButton = IsIPFormat(textIP) && IsPortFormat(textPort);
	}

	panel2ButtonConfirm->Enable(enableButton);
}

void RemoteControlDraft::OnPanelSenderFeaturesChanged(wxCommandEvent& evt)
{
	int selection = evt.GetInt();

	if (panelSenderOptionsLSS->IsShown()) {
		panelSenderOptionsLSS->SetSelection(0);
		panelSenderOptionsLSS->Hide();
	}
	else if (panelSenderOptionsTD->IsShown()) {
		panelSenderOptionsTD->SetSelection(0);
		panelSenderOptionsTD->Hide();
	}

	switch (selection) {
	case 0: case 1:
		panelSenderOptionsLSS->Show();
		break;
	case 2:
		panelSenderOptionsTD->Show();
		break;
	}

	panelSender->Layout();
}

void RemoteControlDraft::OnPanelAuthorizationButtonYesClicked(wxCommandEvent& evt) {
	panelAuthorizationTextAsk->Hide();
	panelAuthorizationButtonYes->Hide();
	panelAuthorizationButtonNo->Hide();
	panelAuthorizationTextAuthorization->Show();
	panelAuthorizationInputField->Show();
	panelAuthorizationButtonConfirm->Show();
	panelAuthorization->Layout();
}
void RemoteControlDraft::OnPanelAuthorizationButtonNoClicked(wxCommandEvent& evt) {
	panelAuthorizationTextAsk->Hide();
	panelAuthorizationButtonYes->Hide();
	panelAuthorizationButtonNo->Hide();
	panelAuthorizationTextFail->Show();
	panelAuthorization->Layout();
	panelAuthorizationTimer->Start(1500, wxTIMER_ONE_SHOT);
}
void RemoteControlDraft::OnPanelAuthorizationButtonConfirmClicked(wxCommandEvent& evt) {
	authorization_code = panelAuthorizationInputField->GetValue();
	std::cout << authorization_code;
	//if (gmailClient.exchangeAuthCodeForAccessToken(authorization_code, access_token, refresh_token))
	//{
	//	// Save access token to file
	//	gmailClient.saveAccessTokenToFile(access_token);
	//	std::cout << "Access token exchanged and saved successfully!" << std::endl;
	//}
	//else
	//{
	//	std::cerr << "Failed to exchange authorization code for access token." << std::endl;
	//}
	//client->loadAccessToken();
	panelAuthorization->Hide();
	panelSender->Show();
	Layout();
}
void RemoteControlDraft::OnPanelAuthorizationClose(wxTimerEvent& evt) {
	panelAuthorizationTimer->Stop();
	Close(true);
}

bool RemoteControlDraft::OnPanelReceiverEventListened() {
	return rand() % 2 == 0;
}
bool RemoteControlDraft::OnPanelReceiverRun() {
	client->run();
	return true;
}

void RemoteControlDraft::OnTimer(wxTimerEvent& event)
{
	// Check the command from the server
	UpdateStatusText();
}

void RemoteControlDraft::UpdateStatusText() {
	bool commandReceived = OnPanelReceiverEventListened();
	if (commandReceived) {
		panelReceiverTextWaiting->Show();
		panelReceiverTextProcessing->Hide();
		panelReceiverTextFeature->Hide();
	}
	else {
		panelReceiverTextWaiting->Hide();
		panelReceiverTextProcessing->Show();
		panelReceiverTextFeature->Show();
	}
	panelReceiver->Layout();
}

// Todo: OnPanelLoginButtonClicked -> Login Gmail, OnPanel2ButtonClicked -> Create socket, OnButtonExitClicked -> Close socket, OnPanelSenderButtonConfirm -> Process command, OnPanelAuthorizationEventListened -> Listen to command,

bool RemoteControlDraft::CreateClient(const std::string& ip_address, int port) {
	client.reset(new Client(ip_address, port));
	if (client) return true;
	return false;
}

bool RemoteControlDraft::CreateEmailClient(const std::string& client_id, const std::string& client_secret, const std::string& redirect_url) {
	gmailClient.reset(new GmailClient(client_id, client_secret, redirect_uri));
	if (gmailClient) return true;
	return false;
}