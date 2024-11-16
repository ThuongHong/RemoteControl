#include <wx/wx.h>
#include <wx/webview.h>
#include <regex>
#include <windows.h>
#include "panel_login.h"

PanelLogin::PanelLogin(wxWindow* parent, wxString description, wxImage image, wxFont headerFont, wxFont mainFont, wxBitmap bitmap) : wxPanel(parent, wxID_ANY)
{
	parent_ = parent;
	Create(description, image);
	Set(headerFont, mainFont, bitmap);
	CreateSizer();
}

void PanelLogin::Create(wxString description, wxImage image)
{
	panelLoginImage = image.Scale(180, 180, wxIMAGE_QUALITY_HIGH);
	roles = { "Sender", "Receiver" };
	panelLoginTextTitle = new wxStaticText(this, wxID_ANY, "PC REMOTE CONTROL", wxDefaultPosition, wxSize(600, -1), wxALIGN_CENTER);
	panelLoginTextDescription = new wxStaticText(this, wxID_ANY, description);
	/*wxStaticText* panelLoginTextMembers = new wxStaticText(panelLogin, wxID_ANY, "Members");
	wxStaticText* panelLoginTextMem1 = new wxStaticText(panelLogin, wxID_ANY, "Chung Tin Dat - 23122024");
	wxStaticText* panelLoginTextMem2 = new wxStaticText(panelLogin, wxID_ANY, "Luu Thuong Hong - 23122006");
	wxStaticText* panelLoginTextMem3 = new wxStaticText(panelLogin, wxID_ANY, "Nguyen Tan Hung - 231220  ");*/
	panelLoginButtonLogin = new wxButton(this, wxID_ANY, "LOGIN WITH GOOGLE", wxDefaultPosition, wxSize(280, 80));
	panelLoginLogo = new wxStaticBitmap(this, wxID_ANY, wxBitmap(panelLoginImage));
}

void PanelLogin::Set(wxFont headerFont, wxFont mainFont, wxBitmap bitmap)
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

void PanelLogin::CreateSizer()
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

	this->SetSizer(panelLoginMainSizer);
}

void PanelLogin::BindControl(wxPanel* desPanel) {
	panelLoginButtonLogin->Bind(wxEVT_BUTTON, [=](wxCommandEvent&) {
		OnButtonClicked(desPanel);
	});
}

void PanelLogin::OnButtonClicked(wxPanel* desPanel)
{
	this->Hide();
	desPanel->Show();
	desPanel->Layout();
	parent_->Layout();
}