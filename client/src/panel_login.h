#pragma once
#include <wx/wx.h>
#include <wx/timer.h>
#include "client.h"
#include "client_socket.h"
#include <memory>

class PanelLogin : public wxPanel
{
public:
	PanelLogin(wxWindow* parent, wxString description, wxImage image, wxFont headerFont, wxFont mainFont, wxBitmap bitmap);
	void BindControl(wxPanel* desPanel);
	void Create(wxString description, wxImage image);
	void Set(wxFont headerFont, wxFont mainFont, wxBitmap bitmap);
	void CreateSizer();
	void OnButtonClicked(wxPanel* desPanel);
private:
	wxWindow* parent_;
	wxStaticText* panelLoginTextTitle;
	wxStaticText* panelLoginTextDescription;
	wxButton* panelLoginButtonLogin;
	wxImage panelLoginImage;
	wxStaticBitmap* panelLoginLogo;
	wxArrayString roles;

	wxBoxSizer* panelLoginMainSizer;
	wxBoxSizer* panelLoginSubSizer1;
	wxBoxSizer* panelLoginSubSizer2;

};