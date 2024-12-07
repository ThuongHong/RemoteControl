#include "RemoteControlDraft.h"
#include <wx/wx.h>
#include <wx/webview.h>
#include <regex>

RemoteControlDraft::RemoteControlDraft(const wxString& title) : wxFrame(nullptr, wxID_ANY, title) {
	wxInitAllImageHandlers();

	/*Create Panel*/
	panel1 = new wxPanel(this);
	panel2 = new wxPanel(this);
	panel2->Hide();
	panel3 = new wxPanel(this);
	panel3->Hide();
	panel4 = new wxPanel(this);
	panel4->Hide();
	panel5 = new wxPanel(this);
	panel5->Hide();

	//Meta Data
	wxString description("Lorem ipsum dolor sit amet, consectetuer adipiscing elit. Maecenas porttitor congue massa. Fusce posuere, magna sed pulvinar ultricies, purus lectus malesuada libero, sit amet commodo magna eros quis urna.\
		Nunc viverra imperdiet enim.Fusce est.Vivamus a tellus.\
		");
	wxFont headerFont(35, wxFONTFAMILY_SWISS, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_NORMAL, false, "Impact"); //Unispace, Impact
	wxFont mainFont(11, wxFONTFAMILY_SWISS, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_NORMAL);
	wxImage image(".\\HCMUS.png", wxBITMAP_TYPE_PNG);
	wxImage icon(".\\google_icon.png", wxBITMAP_TYPE_PNG);
	icon = icon.Scale(50, 50, wxIMAGE_QUALITY_HIGH);
	wxBitmap bitmap(icon);

	// wxWebView* webView = wxWebView::New(this, wxID_ANY, "https://mail.google.com/", wxDefaultPosition, wxSize(800, 600));

	/*PANEL 1*/
	CreatePanel1(description, image);
	SetPanel1(headerFont, mainFont, bitmap);
	CreateSizerPanel1();

	/*PANEL 2*/
	CreatePanel2(description, image);
	SetPanel2(headerFont, mainFont);
	CreateSizerPanel2();

	/*PANEL 3*/
	CreatePanel3(image);
	SetPanel3(headerFont, mainFont);
	CreateSizerPanel3();

	/*PANEL 4*/
	CreatePanel4();
	SetPanel4(headerFont, mainFont);
	CreateSizerPanel4();

	BindControl();
	/**/
	sizerMain = new wxBoxSizer(wxVERTICAL);
	sizerMain->Add(panel1, 1, wxEXPAND);
	sizerMain->Add(panel2, 1, wxEXPAND);
	sizerMain->Add(panel3, 1, wxEXPAND);
	sizerMain->Add(panel4, 1, wxEXPAND);

	this->SetSizer(sizerMain);
}

void RemoteControlDraft::CreatePanel1(wxString description, wxImage image) {
	panel1Image = image.Scale(180, 180, wxIMAGE_QUALITY_HIGH);
	roles = { "Sender", "Receiver" };
	panel1ProgramTitle = new wxStaticText(panel1, wxID_ANY, "PC REMOTE CONTROL", wxDefaultPosition, wxSize(600, -1), wxALIGN_CENTER);
	panel1ProgramDescription = new wxStaticText(panel1, wxID_ANY, description);
	panel1ButtonLogin = new wxButton(panel1, wxID_ANY, "LOGIN WITH GOOGLE", wxDefaultPosition, wxSize(280, 80));
	panel1Logo = new wxStaticBitmap(panel1, wxID_ANY, wxBitmap(panel1Image));
}
void RemoteControlDraft::SetPanel1(wxFont headerFont, wxFont mainFont, wxBitmap bitmap)
{
	panel1ProgramTitle->SetFont(headerFont);
	panel1ProgramTitle->SetBackgroundColour(wxColour(255, 228, 196));
	panel1ProgramDescription->SetFont(mainFont);
	panel1ProgramDescription->Wrap(300);
	panel1ButtonLogin->SetFont(mainFont);
	panel1ButtonLogin->SetBitmap(bitmap);
	panel1ButtonLogin->SetBitmapPosition(wxLEFT);
	panel1ButtonLogin->SetBitmapMargins(20, 0);
}
void RemoteControlDraft::CreateSizerPanel1()
{
	panel1MainSizer = new wxBoxSizer(wxVERTICAL);
	panel1SubSizer1 = new wxBoxSizer(wxHORIZONTAL);
	panel1SubSizer2 = new wxBoxSizer(wxHORIZONTAL);

	panel1SubSizer1->Add(panel1ProgramDescription, 0, wxLEFT, 20);

	panel1SubSizer2->Add(panel1Logo);
	panel1SubSizer2->Add(panel1ButtonLogin, 1, wxALIGN_CENTER_VERTICAL | wxLEFT, 50);

	panel1MainSizer->Add(panel1ProgramTitle, 0, wxALIGN_CENTER_HORIZONTAL | wxBOTTOM, 10);
	panel1MainSizer->Add(panel1SubSizer1);
	panel1MainSizer->Add(panel1SubSizer2);

	panel1->SetSizer(panel1MainSizer);
}

void RemoteControlDraft::CreatePanel2(wxString description, wxImage image)
{
	panel2ProgramTitle = new wxStaticText(panel2, wxID_ANY, "PC REMOTE CONTROL", wxDefaultPosition, wxSize(600, -1), wxALIGN_CENTER);
	panel2ProgramDescription = new wxStaticText(panel2, wxID_ANY, description);
	panel2SubText = new wxStaticText(panel2, wxID_ANY, "Receiver Email");
	panel2Roles = new wxRadioBox(panel2, wxID_ANY, "Choose your role:", wxDefaultPosition, wxDefaultSize, roles, 2, wxRA_SPECIFY_ROWS);
	panel2InputField = new wxTextCtrl(panel2, wxID_ANY, "", wxDefaultPosition, wxSize(200, -1));
	panel2ButtonConfirm = new wxButton(panel2, wxID_ANY, "CONFIRM", wxDefaultPosition, wxSize(150, -1));
}
void RemoteControlDraft::SetPanel2(wxFont headerFont, wxFont mainFont)
{
	panel2ProgramTitle->SetFont(headerFont);
	panel2ProgramTitle->SetBackgroundColour(wxColour(255, 228, 196));
	panel2ProgramDescription->SetFont(mainFont);
	panel2ProgramDescription->Wrap(300);
	panel2Roles->SetFont(mainFont);
	panel2SubText->SetFont(mainFont);
	panel2ButtonConfirm->SetFont(mainFont);
	panel2ButtonConfirm->Disable();
}
void RemoteControlDraft::CreateSizerPanel2()
{
	panel2MainSizer = new wxBoxSizer(wxVERTICAL);
	panel2SubSizer1 = new wxBoxSizer(wxHORIZONTAL);
	panel2SubSizer2 = new wxBoxSizer(wxVERTICAL);

	panel2SubSizer2->Add(panel2SubText, 0, wxALIGN_CENTER_HORIZONTAL);
	panel2SubSizer2->AddSpacer(20);
	panel2SubSizer2->Add(panel2InputField, 0, wxEXPAND);

	panel2SubSizer1->Add(panel2Roles, 0, wxLEFT, 80);
	panel2SubSizer1->AddSpacer(80);
	panel2SubSizer1->Add(panel2SubSizer2);

	panel2MainSizer->Add(panel2ProgramTitle, 0, wxALIGN_CENTER_HORIZONTAL | wxBOTTOM, 10);
	panel2MainSizer->Add(panel2ProgramDescription, 0, wxLEFT, 20);
	panel2MainSizer->AddSpacer(20);
	panel2MainSizer->Add(panel2SubSizer1);
	panel2MainSizer->Add(panel2ButtonConfirm, 0, wxALIGN_CENTER_HORIZONTAL | wxTOP, 20);

	panel2->SetSizer(panel2MainSizer);
}

void RemoteControlDraft::CreatePanel3(wxImage image)
{
	panel3Image = image.Scale(120, 120, wxIMAGE_QUALITY_HIGH);
	features = { "Application", "Service", "File", "Screen Capture", "Webcam", "Shutdown", "Logout" };
	optionsLSS = { "List", "Start", "Stop" };
	optionsTD = { "Take", "Delete" };

	panel3Sender = new wxStaticText(panel3, wxID_ANY, "ROLE: SENDER");
	panel3Features = new wxRadioBox(panel3, wxID_ANY, "Features", wxDefaultPosition, wxDefaultSize, features, 7, wxRA_SPECIFY_ROWS);
	panel3OptionsLSS = new wxRadioBox(panel3, wxID_ANY, "Options", wxDefaultPosition, wxDefaultSize, optionsLSS, 3, wxRA_SPECIFY_COLS);
	panel3OptionsTD = new wxRadioBox(panel3, wxID_ANY, "Options", wxDefaultPosition, wxDefaultSize, optionsTD, 2, wxRA_SPECIFY_COLS);
	panel3ButtonConfirm = new wxButton(panel3, wxID_ANY, "Confirm");
	panel3ButtonExit = new wxButton(panel3, wxID_ANY, "Exit");
	panel3ImageDisplay = new wxStaticBitmap(panel3, wxID_ANY, wxBitmap(panel3Image));
}
void RemoteControlDraft::SetPanel3(wxFont headerFont, wxFont mainFont) {
	panel3Sender->SetFont(headerFont);
	panel3Features->SetFont(mainFont);
	panel3OptionsLSS->SetFont(mainFont);
	panel3OptionsTD->SetFont(mainFont);
	panel3OptionsTD->Hide();
	panel3ButtonConfirm->SetFont(mainFont);
	panel3ButtonExit->SetFont(mainFont);
}
void RemoteControlDraft::CreateSizerPanel3()
{
	panel3MainSizer = new wxBoxSizer(wxVERTICAL);
	panel3SubSizer1 = new wxBoxSizer(wxHORIZONTAL);
	panel3SubSizer2 = new wxBoxSizer(wxHORIZONTAL);
	panel3SubSizer3 = new wxBoxSizer(wxHORIZONTAL);
	panel3SubSizer4 = new wxBoxSizer(wxVERTICAL);

	panel3SubSizer3->Add(panel3ButtonConfirm);
	panel3SubSizer3->AddSpacer(30);
	panel3SubSizer3->Add(panel3ButtonExit);

	panel3SubSizer4->Add(panel3OptionsLSS);
	panel3SubSizer4->Add(panel3OptionsTD);
	panel3SubSizer4->AddSpacer(30);
	panel3SubSizer4->Add(panel3SubSizer3);

	panel3SubSizer1->AddSpacer(30);
	panel3SubSizer1->Add(panel3Sender, 0, wxTOP, 25);
	panel3SubSizer1->AddSpacer(100);
	panel3SubSizer1->Add(panel3ImageDisplay);

	panel3SubSizer2->AddSpacer(50);
	panel3SubSizer2->Add(panel3Features);
	panel3SubSizer2->AddSpacer(100);
	panel3SubSizer2->Add(panel3SubSizer4, 0, wxALIGN_CENTER);

	panel3MainSizer->Add(panel3SubSizer1);
	panel3MainSizer->Add(panel3SubSizer2);
	panel3->SetSizer(panel3MainSizer);
}

void RemoteControlDraft::CreatePanel4()
{
	panel4Receiver = new wxStaticText(panel4, wxID_ANY, "ROLE: RECEIVER");
	panel4ButtonExit = new wxButton(panel4, wxID_ANY, "Exit");
	panel4ImageDisplay = new wxStaticBitmap(panel4, wxID_ANY, wxBitmap(panel3Image));
	panel4TextWaiting = new wxStaticText(panel4, wxID_ANY, "WAITING FOR COMMAND");
	panel4TextProcessing = new wxStaticText(panel4, wxID_ANY, "CURRENT PROCESSING:");
	panel4TextFeature = new wxStaticText(panel4, wxID_ANY, "FEATURE");
	panel4Timer = new wxTimer(panel4, wxID_ANY);
	panel4Timer->Start(2000);
}
void RemoteControlDraft::SetPanel4(wxFont headerFont, wxFont mainFont)
{
	panel4Receiver->SetFont(headerFont);
	panel4TextWaiting->SetFont(headerFont);
	panel4TextProcessing->SetFont(headerFont);
	panel4TextFeature->SetFont(headerFont);
	panel4ButtonExit->SetFont(mainFont);
	panel4TextProcessing->Hide();
	panel4TextFeature->Hide();
}
void RemoteControlDraft::CreateSizerPanel4()
{
	panel4MainSizer = new wxBoxSizer(wxVERTICAL);
	panel4SubSizer1 = new wxBoxSizer(wxHORIZONTAL);

	panel4SubSizer1->AddSpacer(30);
	panel4SubSizer1->Add(panel4Receiver, 0, wxTOP, 25);
	panel4SubSizer1->AddSpacer(100);
	panel4SubSizer1->Add(panel4ImageDisplay);

	panel4MainSizer->Add(panel4SubSizer1);
	panel4MainSizer->Add(panel4TextWaiting, 1, wxALIGN_CENTER | wxTOP | wxBOTTOM, 40);
	panel4MainSizer->Add(panel4TextProcessing, 1, wxALIGN_CENTER);
	panel4MainSizer->Add(panel4TextFeature, 1, wxALIGN_CENTER);
	panel4MainSizer->AddSpacer(30);
	panel4MainSizer->Add(panel4ButtonExit, 0, wxALIGN_CENTER | wxBOTTOM, 30);
	panel4->SetSizer(panel4MainSizer);
}

void RemoteControlDraft::BindControl()
{
	panel1ButtonLogin->Bind(wxEVT_BUTTON, &RemoteControlDraft::OnPanel1ButtonClicked, this);

	panel2Roles->Bind(wxEVT_RADIOBOX, &RemoteControlDraft::OnPanel2RolesChanged, this);
	panel2ButtonConfirm->Bind(wxEVT_BUTTON, &RemoteControlDraft::OnPanel2ButtonClicked, this);
	panel2InputField->Bind(wxEVT_TEXT, &RemoteControlDraft::OnPanel2TextCtrlChanged, this);

	panel3Features->Bind(wxEVT_RADIOBOX, &RemoteControlDraft::OnPanel3FeaturesChanged, this);
	panel3ButtonExit->Bind(wxEVT_BUTTON, &RemoteControlDraft::OnButtonExitClicked, this);
	panel3ButtonConfirm->Bind(wxEVT_BUTTON, &RemoteControlDraft::OnPanel3ButtonConfirm, this);
	panel4ButtonExit->Bind(wxEVT_BUTTON, &RemoteControlDraft::OnButtonExitClicked, this);
	panel4->Bind(wxEVT_TIMER, &RemoteControlDraft::OnTimer, this, panel4Timer->GetId());
}

void RemoteControlDraft::OnPanel1ButtonClicked(wxCommandEvent& evt)
{
	//Open webview to sign in gmail
	panel1->Hide();
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
		panel3->Show();
		break;
	case 1: // To RECEIVER PANEL
		//Connect receiver to server
		panel2->Hide();
		panel4->Show();
		break;
	}
	Layout();
	panel4->Layout();
	panel4->Refresh();
}
void RemoteControlDraft::OnButtonExitClicked(wxCommandEvent& evt)
{
	//Close socket
	Close(true);
	panel4Timer->Stop();
}

void RemoteControlDraft::OnPanel3ButtonConfirm(wxCommandEvent& evt)
{
	int selection1 = panel3Features->GetSelection();
	int selection2 = panel3OptionsLSS->GetSelection();
	int selection3 = panel3OptionsTD->GetSelection();

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

void RemoteControlDraft::OnPanel2RolesChanged(wxCommandEvent& evt)
{
	int selection = evt.GetInt();
	switch (selection) {
	case 0:
		panel2SubText->SetLabel("Receiver Email");
		break;
	case 1:
		panel2SubText->SetLabel("Server IP");
		break;
	}
	panel2InputField->Clear();
	panel2SubSizer2->Layout();
}
void RemoteControlDraft::OnPanel2TextCtrlChanged(wxCommandEvent& event) {
	wxString text = panel2InputField->GetValue();
	bool enableButton = false;

	if (panel2Roles->GetSelection() == 0) {  // Gmail
		enableButton = IsEmailFormat(text);
	}
	else if (panel2Roles->GetSelection() == 1) {  // IP
		enableButton = IsIPFormat(text);
	}

	panel2ButtonConfirm->Enable(enableButton);
}
void RemoteControlDraft::OnPanel3FeaturesChanged(wxCommandEvent& evt)
{
	int selection = evt.GetInt();

	if (panel3OptionsLSS->IsShown()) {
		panel3OptionsLSS->SetSelection(0);
		panel3OptionsLSS->Hide();
	}
	else if (panel3OptionsTD->IsShown()) {
		panel3OptionsTD->SetSelection(0);
		panel3OptionsTD->Hide();
	}

	switch (selection) {
	case 0: case 1:
		panel3OptionsLSS->Show();
		break;
	case 2:
		panel3OptionsTD->Show();
		break;
	}

	panel3->Layout();
}

bool RemoteControlDraft::OnPanel4EventListened() {
	return rand() % 2 == 0;
}


void RemoteControlDraft::OnTimer(wxTimerEvent& event)
{
	// Check the command from the server
	UpdateStatusText();
}

void RemoteControlDraft::UpdateStatusText() {
	bool commandReceived = OnPanel4EventListened();
	if (commandReceived) {
		panel4TextWaiting->Show();
		panel4TextProcessing->Hide();
		panel4TextFeature->Hide();
	}
	else {
		panel4TextWaiting->Hide();
		panel4TextProcessing->Show();
		panel4TextFeature->Show();
	}
	panel4->Layout();
}

// Todo: OnPanel1ButtonClicked -> Login Gmail, OnPanel2ButtonClicked -> Create socket, OnButtonExitClicked -> Close socket, OnPanel3ButtonConfirm -> Process command, OnPanel4EventListened -> Listen to command,