#include "RemoteControlDraft.h"
#include <wx/wx.h>
#include <wx/webview.h>
#include <regex>

RemoteControlDraft::RemoteControlDraft(const wxString& title) : wxFrame(nullptr, wxID_ANY, title) {
	wxInitAllImageHandlers();

	/*Create Panel*/

	panel1 = new wxPanel(this);
	panel1->Hide();
	panel2 = new wxPanel(this);
	panel2->Hide();
	panel3 = new wxPanel(this);
	//panel3->Hide();

	// Meta Data
	wxString description("Lorem ipsum dolor sit amet, consectetuer adipiscing elit. Maecenas porttitor congue massa. Fusce posuere, magna sed pulvinar ultricies, purus lectus malesuada libero, sit amet commodo magna eros quis urna.\
		Nunc viverra imperdiet enim.Fusce est.Vivamus a tellus.\
		");
	wxFont headerFont(35, wxFONTFAMILY_SWISS, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_NORMAL, false, "Impact"); //Unispace, Impact
	wxFont mainFont(11, wxFONTFAMILY_SWISS, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_NORMAL);
	wxImage image(".\\HCMUS.png", wxBITMAP_TYPE_PNG);
	panel1Image = image.Scale(180, 180, wxIMAGE_QUALITY_HIGH);
	wxImage icon(".\\google_icon.png", wxBITMAP_TYPE_PNG);
	icon = icon.Scale(50, 50, wxIMAGE_QUALITY_HIGH);
	wxBitmap bitmap(icon);
	wxArrayString roles;
	roles.Add("Sender");
	roles.Add("Receiver");

	/*PANEL 1*/

	panel1ProgramTitle = new wxStaticText(panel1, wxID_ANY, "PC REMOTE CONTROL", wxDefaultPosition, wxSize(600, -1), wxALIGN_CENTER);
	panel1ProgramDescription = new wxStaticText(panel1, wxID_ANY, description);
	panel1ButtonLogin = new wxButton(panel1, wxID_ANY, "LOGIN WITH GOOGLE", wxDefaultPosition, wxSize(280, 80));
	panel1Logo = new wxStaticBitmap(panel1, wxID_ANY, wxBitmap(panel1Image));

	// wxWebView* webView = wxWebView::New(this, wxID_ANY, "https://mail.google.com/", wxDefaultPosition, wxSize(800, 600));

	// Modify
	panel1ProgramTitle->SetFont(headerFont);
	panel1ProgramTitle->SetBackgroundColour(wxColour(255, 228, 196));
	panel1ProgramDescription->SetFont(mainFont);
	panel1ProgramDescription->Wrap(300);
	panel1ButtonLogin->SetFont(mainFont);
	panel1ButtonLogin->SetBitmap(bitmap);
	panel1ButtonLogin->SetBitmapPosition(wxLEFT);
	panel1ButtonLogin->SetBitmapMargins(30, 0);

	// Bind Event
	panel1ButtonLogin->Bind(wxEVT_BUTTON, &RemoteControlDraft::OnPanel1ButtonSwitch, this);

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

	/*PANEL 2*/
	panel2ProgramTitle = new wxStaticText(panel2, wxID_ANY, "PC REMOTE CONTROL", wxDefaultPosition, wxSize(600, -1), wxALIGN_CENTER);
	panel2ProgramDescription = new wxStaticText(panel2, wxID_ANY, description);
	panel2SubText = new wxStaticText(panel2, wxID_ANY, "Receiver Email");
	panel2Roles = new wxRadioBox(panel2, wxID_ANY, "Choose your role:", wxDefaultPosition, wxDefaultSize, roles, 2, wxRA_SPECIFY_ROWS);
	panel2InputField = new wxTextCtrl(panel2, wxID_ANY, "", wxDefaultPosition, wxSize(200, -1));
	panel2ButtonConfirm = new wxButton(panel2, wxID_ANY, "CONFIRM", wxDefaultPosition, wxSize(150, -1));

	panel2ProgramTitle->SetFont(headerFont);
	panel2ProgramTitle->SetBackgroundColour(wxColour(255, 228, 196));
	panel2ProgramDescription->SetFont(mainFont);
	panel2ProgramDescription->Wrap(300);
	panel2Roles->SetFont(mainFont);
	panel2SubText->SetFont(mainFont);
	panel2ButtonConfirm->SetFont(mainFont);
	panel2ButtonConfirm->Disable();

	panel2Roles->Bind(wxEVT_RADIOBOX, &RemoteControlDraft::OnPanel2RadioBoxChanged, this);
	panel2ButtonConfirm->Bind(wxEVT_BUTTON, &RemoteControlDraft::OnPanel2ButtonSwitch, this);
	panel2InputField->Bind(wxEVT_TEXT, &RemoteControlDraft::OnPanel2TextCtrlChanged, this);

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

	/*PANEL 3*/
	panel3Image = image.Scale(120, 120, wxIMAGE_QUALITY_HIGH);
	features = { "Application", "Service", "File", "Screen Capture", "Webcam", "Shutdown", "Logout"};
	optionsLSS = { "List", "Start", "Stop" };
	optionsTD = { "Take", "Delete" };

	panel3Sender = new wxStaticText(panel3, wxID_ANY, "ROLE: SENDER");
	panel3Features = new wxRadioBox(panel3, wxID_ANY, "Features", wxDefaultPosition, wxDefaultSize, features, 7, wxRA_SPECIFY_ROWS);
	panel3Options = new wxRadioBox(panel3, wxID_ANY, "Options", wxDefaultPosition, wxDefaultSize, optionsLSS, 3, wxRA_SPECIFY_COLS);
	panel3SubText = new wxStaticText(panel3, wxID_ANY, "Choose Directory to Save Result");
	panel3Directory = new wxTextCtrl(panel3, wxID_ANY, "Default: .\\result", wxDefaultPosition, wxSize(200, -1));
	panel3ButtonBrowse = new wxButton(panel3, wxID_ANY, "Browse");
	panel3ButtonConfirm = new wxButton(panel3, wxID_ANY, "Confirm");
	panel3ButtonExit = new wxButton(panel3, wxID_ANY, "Exit");
	panel3ImageDisplay = new wxStaticBitmap(panel3, wxID_ANY, wxBitmap(panel3Image));

	panel3Features->Bind(wxEVT_RADIOBOX, &RemoteControlDraft::OnPanel3FeaturesChanged, this);

	panel3Sender->SetFont(headerFont);
	panel3Features->SetFont(mainFont);
	panel3Options->SetFont(mainFont);
	panel3SubText->SetFont(mainFont);
	panel3Directory->SetFont(mainFont);
	panel3ButtonBrowse->SetFont(mainFont);
	panel3ButtonConfirm->SetFont(mainFont);
	panel3ButtonExit->SetFont(mainFont);
	

	wxBoxSizer* panel3MainSizer = new wxBoxSizer(wxVERTICAL);
	wxBoxSizer* panel3SubSizer1 = new wxBoxSizer(wxHORIZONTAL);
	wxBoxSizer* panel3SubSizer2 = new wxBoxSizer(wxHORIZONTAL);
	wxBoxSizer* panel3SubSizer3 = new wxBoxSizer(wxVERTICAL);
	wxBoxSizer* panel3SubSizer4 = new wxBoxSizer(wxVERTICAL);
	wxBoxSizer* panel3SubSizer5 = new wxBoxSizer(wxHORIZONTAL);
	wxBoxSizer* panel3SubSizer6 = new wxBoxSizer(wxHORIZONTAL);
	
	panel3SubSizer1->AddSpacer(30);
	panel3SubSizer1->Add(panel3Sender, 0, wxTOP, 25);
	panel3SubSizer1->AddSpacer(100);
	panel3SubSizer1->Add(panel3ImageDisplay);

	panel3SubSizer3->Add(panel3Features);

	panel3SubSizer4->Add(panel3Options);
	panel3SubSizer4->AddSpacer(30);
	panel3SubSizer4->Add(panel3SubText, 0, wxLEFT, 20);

	panel3SubSizer5->Add(panel3Directory);
	panel3SubSizer5->AddSpacer(10);
	panel3SubSizer5->Add(panel3ButtonBrowse);

	panel3SubSizer6->Add(panel3ButtonConfirm);
	panel3SubSizer6->AddSpacer(30);
	panel3SubSizer6->Add(panel3ButtonExit);

	panel3SubSizer4->Add(panel3SubSizer5);
	panel3SubSizer4->AddSpacer(50);
	panel3SubSizer4->Add(panel3SubSizer6);

	panel3SubSizer2->AddSpacer(50);
	panel3SubSizer2->Add(panel3SubSizer3);
	panel3SubSizer2->AddSpacer(50);
	panel3SubSizer2->Add(panel3SubSizer4);

	panel3MainSizer->Add(panel3SubSizer1);
	panel3MainSizer->Add(panel3SubSizer2);
	panel3->SetSizer(panel3MainSizer);

	/**/
	sizerMain = new wxBoxSizer(wxVERTICAL);
	sizerMain->Add(panel1, 1, wxEXPAND);
	sizerMain->Add(panel2, 1, wxEXPAND);
	sizerMain->Add(panel3, 1, wxEXPAND);

	this->SetSizer(sizerMain);
}

void RemoteControlDraft::OnPanel1ButtonSwitch(wxCommandEvent& evt)
{
	panel1->Hide();
	panel2->Show();
	Layout();
}

void RemoteControlDraft::OnPanel2ButtonSwitch(wxCommandEvent& evt)
{
	panel2->Hide();
	panel3->Show();
	Layout();
}

void RemoteControlDraft::OnPanel2RadioBoxChanged(wxCommandEvent& evt)
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

bool RemoteControlDraft::IsEmailFormat(const wxString& text)
{
	const std::regex emailPattern(R"((\w+)(\.\w+)*@gmail\.com)");
	return std::regex_match(text.ToStdString(), emailPattern);
}

bool RemoteControlDraft::IsIPFormat(const wxString& text) {
	const std::regex ipPattern("^((25[0-5]|2[0-4][0-9]|[01]?[0-9][0-9]?)\\.){3}(25[0-5]|2[0-4][0-9]|[01]?[0-9][0-9]?)$");
	return std::regex_match(text.ToStdString(), ipPattern);
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
	switch (selection) {
	case 0: case 1:
		panel3Options->Show();
		for (int i = 0; i < optionsLSS.GetCount(); i++) {
			panel3Options->SetString(i, optionsLSS[i]);
		}
		panel3Options->Refresh();
		break;
	case 2:
		panel3Options->Show();
		for (int i = 0; i < optionsTD.GetCount(); i++) {
			panel3Options->SetString(i, optionsTD[i]);
		}
		panel3Options->Refresh();
		break;
	default:
		panel3Options->Hide();
		break;
	}
	panel3SubSizer4->Layout();
}
