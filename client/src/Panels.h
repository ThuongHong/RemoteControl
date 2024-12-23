#pragma once
#include <wx/wx.h>
#include <wx/timer.h>
#include <wx/webview.h>
#include <wx/listctrl.h>
#include <curl/curl.h>
#include <memory>
#include <regex>
#include "Client.h"
#include "HTTPListener.h"

class PanelLogin : public wxPanel
{
public:
	PanelLogin(wxWindow *parent, wxString description, wxImage image, wxFont headerFont, wxFont mainFont, wxBitmap bitmap);
	void BindControl(wxPanel *desPanel, std::string redirect_uri, std::string client_id, std::string &access_token, std::string &refresh_token, wxScopedPtr<OAuth2Handler> &oAuth2Handler);

private:
	wxWindow *parent_;

	wxStaticText *TextTitle;
	wxStaticText *TextDescription;
	wxStaticText *TextMem1;
	wxStaticText *TextMem2;
	wxStaticText *TextMem3;
	wxButton *ButtonLogin;
	wxStaticBitmap *Logo;

	void Create(wxString description, wxImage image);
	void Set(wxFont headerFont, wxFont mainFont, wxBitmap bitmap);
	void CreateSizer();
	void OnButtonClicked(wxPanel *desPanel, std::string redirect_uri, std::string client_id, std::string &access_token, std::string &refresh_token, wxScopedPtr<OAuth2Handler> &oAuth2Handler);
};

class PanelRoles : public wxPanel
{
public:
	PanelRoles(wxWindow *parent, wxString description, wxFont headerFont, wxFont mainFont, wxBitmap bitmap);
	void BindControl(wxPanel *desPanel1, wxPanel *desPanel2, std::string &ip_address, int &port, std::string &receive_email, std::string &send_email, wxStaticText *m_statusText, wxScopedPtr<Client> &client, std::string &access_token, wxScopedPtr<GmailSender> &gmailSender, std::vector<std::string> &tasks, wxScopedPtr<GmailReceiver> &gmailReceiver);

private:
	wxWindow *parent_;

	wxStaticText *TextTitle;
	wxStaticText *TextDescription;
	wxStaticText *TextMem1;
	wxStaticText *TextMem2;
	wxStaticText *TextMem3;
	wxStaticText *TextEmail;
	wxStaticText *TextIP;
	wxStaticText *TextPort;
	wxRadioBox *Roles;
	wxTextCtrl *InputFieldEmail;
	wxTextCtrl *InputFieldIP;
	wxTextCtrl *InputFieldPort;
	wxButton *ButtonConfirm;

	void Create(wxString description);
	void Set(wxFont headerFont, wxFont mainFont);
	void CreateSizer();

	void OnButtonClicked(wxPanel *desPanel1, wxPanel *desPanel2, std::string &ip_address, int &port, std::string &receive_email, std::string &send_email, wxStaticText *m_statusText, wxScopedPtr<Client> &client, std::string &access_token, wxScopedPtr<GmailSender> &gmailSender, std::vector<std::string> &tasks, wxScopedPtr<GmailReceiver> &gmailReceiver);
	void OnRolesChanged(wxCommandEvent &evt);
	void OnTextCtrlChanged(wxCommandEvent &event);

	bool IsEmailFormat(const wxString &text);
	bool IsIPFormat(const wxString &text);
	bool IsPortFormat(const wxString &text);

	bool CreateClient(std::string &ip_address, int &port, wxStaticText *m_statusText, wxScopedPtr<Client> &client, std::vector<std::string> &tasks);
	bool CreateEmailSender(const std::string &access_token, wxScopedPtr<GmailSender> &gmailSender);
	bool CreateEmailReceiver(const std::string &access_token, wxScopedPtr<GmailReceiver> &gmailReceiver);
};

class PanelExplorer : public wxPanel
{
public:
	PanelExplorer(wxWindow *parent, int &processID);
	bool PopulateTableFromFile(const std::wstring &filePath, int format);
	void BindControl(wxPanel *desPanel, int &processID, std::string &filename, wxScopedPtr<GmailSender> &gmailSender);
	void ReconstructTable(int format);

private:
	wxWindow *parent_;
	int format_ = 0;

	wxListCtrl *table;
	wxButton *ButtonAction;
	wxButton* ButtonReturn;
	wxTimer *Timer;

	void Create();
	void CreateSizer();

	void OnButtonActionClicked(int &processID, std::string &filename, wxScopedPtr<GmailSender> &gmailSender);
	void OnButtonReturnClicked(wxPanel *desPanel);
	void OnTimer(wxTimerEvent &evt);
};

class PanelSender : public wxPanel
{
public:
	PanelSender(wxWindow *parent, wxImage image, wxFont headerFont, wxFont mainFont);
	void BindControl(PanelExplorer *panelExplorer, std::string &file_name, std::string &app_svc_name, int &processID, std::string receive_email, wxScopedPtr<GmailSender> &gmailSender);

private:
	wxWindow *parent_;

	wxStaticText *TextTitle;
	wxRadioBox *Features;
	wxRadioBox *OptionsAppSvc;
	wxRadioBox *OptionsFile;
	wxRadioBox *OptionsCamera;
	wxButton *ButtonConfirm;
	wxButton *ButtonExit;
	wxStaticBitmap *ImageDisplay;
	wxTextCtrl *InputFieldProcessID;
	wxTextCtrl *InputFieldAppSvcName;
	wxTextCtrl *InputFieldFileName;

	void Create(wxImage image);
	void Set(wxFont headerFont, wxFont mainFont);
	void CreateSizer();
	void OnFeaturesChanged(wxCommandEvent &evt);
	void OnOptionsChanged(wxCommandEvent &evt);
	void OnButtonConfirmClicked(PanelExplorer *panelExplorer, std::string &file_name, std::string &app_svc_name, int &processID, std::string receive_email, wxScopedPtr<GmailSender> &gmailSender);
	void OnButtonCloseClicked();
	void OnButtonExitClicked(wxCommandEvent &evt);
};

class PanelReceiver : public wxPanel
{
public:
	PanelReceiver(wxWindow *parent, wxImage image, wxFont headerFont, wxFont mainFont, wxStaticText *m_statusText);
	void BindControl(wxScopedPtr<Client> &client);
	void CreateSizer(wxStaticText *m_statusText);

private:
	wxWindow *parent_;

	wxStaticText *TextTitle;
	wxButton *ButtonExit;
	wxStaticBitmap *ImageDisplay;
	Client *m_client;
	wxTextCtrl *inputMessage;

	void Create(wxImage image);
	void Set(wxFont headerFont, wxFont mainFont);
	void OnButtonExitClicked(wxScopedPtr<Client> &client);
	void OnUpdateTimer(wxTimerEvent &event);
};
