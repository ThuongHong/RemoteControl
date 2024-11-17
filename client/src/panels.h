#pragma once
#include <wx/wx.h>
#include <wx/timer.h>
#include "client.h"
#include "client_socket.h"
#include <memory>
#include <regex>

class PanelLogin : public wxPanel
{
public:
	PanelLogin(wxWindow* parent, wxString description, wxImage image, wxFont headerFont, wxFont mainFont, wxBitmap bitmap);
	void BindControl(wxPanel* desPanel);

private:
	wxWindow* parent_;

	wxStaticText* TextTitle;
	wxStaticText* TextDescription;
	wxButton* ButtonLogin;
	wxImage Image;
	wxStaticBitmap* Logo;
	wxArrayString roles;

	wxBoxSizer* MainSizer;
	wxBoxSizer* SubSizer1;
	wxBoxSizer* SubSizer2;
	
	void Create(wxString description, wxImage image);
	void Set(wxFont headerFont, wxFont mainFont, wxBitmap bitmap);
	void CreateSizer();
	void OnButtonClicked(wxPanel* desPanel);
};

class PanelRoles : public wxPanel
{
public:
	PanelRoles(wxWindow* parent, wxString description, wxFont headerFont, wxFont mainFont, wxBitmap bitmap);
	void BindControl(wxPanel* desPanel1, wxPanel* desPanel2, std::string &ip_address, int &port, std::string &target_email, wxScopedPtr<Client> &client);

private:
	wxWindow* parent_;

	wxStaticText* TextTitle;
	wxStaticText* TextDescription;
	wxStaticText* TextEmail;
	wxStaticText* TextIP;
	wxStaticText* TextPort;
	wxRadioBox* Roles;
	wxTextCtrl* InputFieldEmail;
	wxTextCtrl* InputFieldIP;
	wxTextCtrl* InputFieldPort;
	wxButton* ButtonConfirm;
	wxArrayString roles;

	wxBoxSizer* MainSizer;
	wxBoxSizer* SubSizer1;
	wxBoxSizer* SubSizer2;

	void Create(wxString description);
	void Set(wxFont headerFont, wxFont mainFont);
	void CreateSizer();

	void OnButtonClicked(wxPanel* desPanel1, wxPanel* desPanel, std::string& ip_address, int& port, std::string &target_email, wxScopedPtr<Client> &client);
	void OnRolesChanged(wxCommandEvent& evt);
	void OnTextCtrlChanged(wxCommandEvent& event);
	
	bool IsEmailFormat(const wxString& text);
	bool IsIPFormat(const wxString& text);
	bool IsPortFormat(const wxString& text);

	bool CreateClient(const std::string& ip_address, int port, wxScopedPtr<Client> &client);
	bool CreateEmailClient(const std::string& client_id, const std::string& client_secret, const std::string& redirect_url, wxScopedPtr<GmailClient> &gmailClient);
};

class PanelAuthorization : public wxPanel
{
public:
	PanelAuthorization(wxWindow* parent, wxFont headerFont, wxFont mainFont);
	void BindControl(wxPanel* desPanel, std::string &authorization_code);

private:
	wxWindow* parent_;

	wxStaticText* TextTitle;
	wxStaticText* TextAsk;
	wxButton* ButtonYes;
	wxButton* ButtonNo;
	wxButton* ButtonConfirm;
	wxButton* ButtonExit;
	wxStaticText* TextAuthorization;
	wxStaticText* TextSuccess;
	wxStaticText* TextFail;
	wxTextCtrl* InputField;
	wxTimer* Timer;

	wxBoxSizer* MainSizer;
	wxBoxSizer* SubSizer1;
	wxBoxSizer* SubSizer2;
	wxBoxSizer* SubSizer3;
	wxBoxSizer* SubSizer4;

	void Create();
	void Set(wxFont headerFont, wxFont mainFont);
	void CreateSizer();

	void OnButtonYesClicked(wxCommandEvent& evt);
	void OnButtonNoClicked(wxCommandEvent& evt);
	void OnButtonExitClicked(wxCommandEvent& evt);
	void OnButtonConfirmClicked(wxPanel* desPanel, std::string& authorization_code);
	void OnClose(wxTimerEvent& evt);

};

class PanelSender : public wxPanel
{
public:
	PanelSender(wxWindow* parent, wxImage image, wxFont headerFont, wxFont mainFont);
	void BindControl();

private:
	wxWindow* parent_;

	wxImage Image;
	wxStaticText* TextTitle;
	wxRadioBox* Features;
	wxRadioBox* OptionsLSS;
	wxRadioBox* OptionsTD;
	wxButton* ButtonConfirm;
	wxButton* ButtonExit;
	wxStaticBitmap* ImageDisplay;

	wxBoxSizer* MainSizer;
	wxBoxSizer* SubSizer1;
	wxBoxSizer* SubSizer2;
	wxBoxSizer* SubSizer3;
	wxBoxSizer* SubSizer4;

	void Create(wxImage image);
	void Set(wxFont headerFont, wxFont mainFont);
	void CreateSizer();
	void OnFeaturesChanged(wxCommandEvent& evt);
	void OnButtonConfirmClicked(wxCommandEvent& evt);
	void OnButtonExitClicked(wxCommandEvent& evt);
};

class PanelReceiver : public wxPanel
{
public:
	PanelReceiver(wxWindow* parent, wxImage image, wxFont headerFont, wxFont mainFont);
	void BindControl();

private:
	wxWindow* parent_;

	wxImage Image;
	wxStaticText* TextTitle;
	wxButton* ButtonExit;
	wxStaticBitmap* ImageDisplay;
	wxStaticText* TextWaiting;
	wxStaticText* TextProcessing;
	wxStaticText* TextFeature;
	wxTimer* Timer;

	wxBoxSizer* MainSizer;
	wxBoxSizer* SubSizer1;

	void Create(wxImage image);
	void Set(wxFont headerFont, wxFont mainFont);
	void CreateSizer();

	bool OnEventListened();
	bool OnRun();
	void OnButtonExitClicked(wxCommandEvent& evt);
	void OnTimer(wxTimerEvent& event);
	void UpdateStatusText();
};
