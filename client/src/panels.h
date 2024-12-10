#pragma once
#include <wx/wx.h>
#include <wx/timer.h>
#include <wx/webview.h>
#include "client.h"
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
	wxStaticText* TextMem1;
	wxStaticText* TextMem2;
	wxStaticText* TextMem3;
	wxButton* ButtonLogin;
	wxImage Image;
	wxStaticBitmap* Logo;
	wxArrayString roles;

	wxBoxSizer* MainSizer;
	wxBoxSizer* SubSizer1;
	wxBoxSizer* SubSizer2;
	wxBoxSizer* SubSizer3;
	
	void Create(wxString description, wxImage image);
	void Set(wxFont headerFont, wxFont mainFont, wxBitmap bitmap);
	void CreateSizer();
	void OnButtonClicked(wxPanel* desPanel);
};

class PanelRoles : public wxPanel
{
public:
	PanelRoles(wxWindow* parent, wxString description, wxFont headerFont, wxFont mainFont, wxBitmap bitmap);
	void BindControl(wxPanel* desPanel1, wxPanel* desPanel2, std::string& ip_address, int &port, std::string &target_email, wxStaticText* m_statusText, wxScopedPtr<Client> &client);

private:
	wxWindow* parent_;

	wxStaticText* TextTitle;
	wxStaticText* TextDescription;
	wxStaticText* TextMem1;
	wxStaticText* TextMem2;
	wxStaticText* TextMem3;
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
	wxBoxSizer* SubSizer3;
	wxBoxSizer* SubSizer4;

	void Create(wxString description);
	void Set(wxFont headerFont, wxFont mainFont);
	void CreateSizer();

	void OnButtonClicked(wxPanel* desPanel1, wxPanel* desPanel2, std::string& ip_address, int& port, std::string &target_email, wxStaticText* m_statusText, wxScopedPtr<Client>& client);
	void OnRolesChanged(wxCommandEvent& evt);
	void OnTextCtrlChanged(wxCommandEvent& event);
	
	bool IsEmailFormat(const wxString& text);
	bool IsIPFormat(const wxString& text);
	bool IsPortFormat(const wxString& text);

	bool CreateClient(std::string& ip_address, int& port, wxStaticText* m_statusText, wxScopedPtr<Client> &client);
};

class PanelAuthorization : public wxPanel
{
public:
	PanelAuthorization(wxWindow* parent, wxFont headerFont, wxFont mainFont);
	void BindControl(wxPanel* desPanel, std::string &authorization_code, std::string &access_token, std::string &refresh_token, wxScopedPtr<Client> &client, wxScopedPtr<GmailClient> &gmailClient);

private:
	wxWindow* parent_;

	wxStaticText* TextTitle;
	wxStaticText* TextHelp;
	//wxStaticText* TextAsk;
	//wxButton* ButtonYes;
	//wxButton* ButtonNo;
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

	/*void OnButtonYesClicked(wxCommandEvent& evt);
	void OnButtonNoClicked(wxCommandEvent& evt);*/
	void OnButtonExitClicked(wxCommandEvent& evt);
	void OnButtonConfirmClicked(wxPanel* desPanel, std::string& authorization_code, std::string &access_token, std::string &refresh_token, wxScopedPtr<Client>& client, wxScopedPtr<GmailClient>& gmailClient);
	void OnClose(wxTimerEvent& evt);
};

class PanelSender : public wxPanel
{
public:
	PanelSender(wxWindow* parent, wxImage image, wxFont headerFont, wxFont mainFont);
	void BindControl(std::string &filename, int &processID);

private:
	wxWindow* parent_;

	wxImage Image;
	wxStaticText* TextTitle;
	wxRadioBox* Features;
	wxRadioBox* OptionsLSS;
	wxRadioBox* OptionsLGD;
	wxButton* ButtonConfirm;
	wxButton* ButtonClose;
	wxButton* ButtonExit;
	wxStaticBitmap* ImageDisplay;
	wxTextCtrl* InputFieldProcessID;
	wxTextCtrl* InputFieldFilename;

	wxBoxSizer* MainSizer;
	wxBoxSizer* SubSizer1;
	wxBoxSizer* SubSizer2;
	wxBoxSizer* SubSizer3;
	wxBoxSizer* SubSizer4;

	void Create(wxImage image);
	void Set(wxFont headerFont, wxFont mainFont);
	void CreateSizer();
	void OnFeaturesChanged(wxCommandEvent& evt);
	void OnOptionsChanged(wxCommandEvent& evt);
	void OnButtonConfirmClicked(std::string& filename, int& processID);
	void OnButtonCloseClicked();
	void OnButtonExitClicked(wxCommandEvent& evt);
};

class PanelReceiver : public wxPanel
{
public:
	PanelReceiver(wxWindow* parent, wxImage image, wxFont headerFont, wxFont mainFont, wxStaticText* m_statusText);
	void BindControl(wxScopedPtr<Client>& client);
	void CreateSizer(wxStaticText* m_statusText);

private:
	wxWindow* parent_;

	wxImage Image;
	wxStaticText* TextTitle;
	wxButton* ButtonExit;
	wxStaticBitmap* ImageDisplay;
	//wxStaticText* TextWaiting;
	//wxStaticText* TextProcessing;
	//wxStaticText* TextFeature;
	Client* m_client;
	wxTextCtrl* inputMessage;
	//wxTimer* Timer;

	wxBoxSizer* MainSizer;
	wxBoxSizer* SubSizer1;

	void Create(wxImage image);
	void Set(wxFont headerFont, wxFont mainFont);

	bool OnEventListened();
	//bool OnRun();
	void OnButtonExitClicked(wxCommandEvent& evt);
	void OnTimer(wxTimerEvent& event);
	void UpdateStatusText();
};
