#pragma once
#include <wx/wx.h>
#include <wx/timer.h>
#include "client.h"
#include "client_socket.h"
#include <memory>
#include "panelLogin.h"

class RemoteControlDraft : public wxFrame
{
public:
	RemoteControlDraft(const wxString& title);
private:
	//GUI
	void CreatePanelLogin(wxString description, wxImage image);
	void SetPanelLogin(wxFont headerFont, wxFont mainFont, wxBitmap bitmap);
	void CreateSizerPanelLogin();

	void CreatePanel2(wxString description, wxImage image);
	void SetPanel2(wxFont headerFont, wxFont mainFont);
	void CreateSizerPanel2();

	void CreatePanelSender(wxImage image);
	void SetPanelSender(wxFont headerFont, wxFont mainFont);
	void CreateSizerPanelSender();

	void CreatePanelAuthorization();
	void SetPanelAuthorization(wxFont headerFont, wxFont mainFont);
	void CreateSizerPanelAuthorization();

	void CreatePanelReceiver();
	void SetPanelReceiver(wxFont headerFont, wxFont mainFont);
	void CreateSizerPanelReceiver();

	void BindControl();

	void OnPanelLoginButtonClicked(wxCommandEvent& evt);

	void OnPanel2ButtonClicked(wxCommandEvent& evt);
	void OnPanel2RolesChanged(wxCommandEvent& evt);
	void OnPanel2TextCtrlChanged(wxCommandEvent& event);

	bool IsEmailFormat(const wxString& text);
	bool IsIPFormat(const wxString& text);
	bool IsPortFormat(const wxString& text);

	void OnPanelSenderFeaturesChanged(wxCommandEvent& evt);
	void OnPanelSenderButtonConfirm(wxCommandEvent& evt);

	void OnPanelAuthorizationButtonYesClicked(wxCommandEvent& evt);
	void OnPanelAuthorizationButtonNoClicked(wxCommandEvent& evt);
	void OnPanelAuthorizationButtonConfirmClicked(wxCommandEvent& evt);
	void OnPanelAuthorizationClose(wxTimerEvent& evt);

	bool OnPanelReceiverEventListened();
	bool OnPanelReceiverRun();
	void OnButtonExitClicked(wxCommandEvent& evt);
	void OnTimer(wxTimerEvent& event);
	void UpdateStatusText();

	bool CreateClient(const std::string& ip_address, int port);
	bool CreateEmailClient(const std::string& client_id, const std::string& client_secret, const std::string& redirect_url);

	PanelLogin* panelLogin;
	//wxPanel* panelLogin;
	wxPanel* panel2;
	wxPanel* panelSender;
	wxPanel* panelAuthorization;
	wxPanel* panelReceiver;

	wxString description;

	wxStaticText* panelLoginTextTitle;
	wxStaticText* panelLoginTextDescription;
	wxButton* panelLoginButtonLogin;
	wxImage panelLoginImage;
	wxStaticBitmap* panelLoginLogo;
	wxArrayString roles;

	wxBoxSizer* panelLoginMainSizer;
	wxBoxSizer* panelLoginSubSizer1;
	wxBoxSizer* panelLoginSubSizer2;

	wxStaticText* panel2TextTitle;
	wxStaticText* panel2TextDescription;
	wxStaticText* panel2SubTextEmail;
	wxStaticText* panel2SubTextIP;
	wxStaticText* panel2SubTextPort;
	wxRadioBox* panel2Roles;
	wxTextCtrl* panel2InputFieldEmail;
	wxTextCtrl* panel2InputFieldIP;
	wxTextCtrl* panel2InputFieldPort;
	wxButton* panel2ButtonConfirm;

	wxBoxSizer* panel2MainSizer;
	wxBoxSizer* panel2SubSizer1;
	wxBoxSizer* panel2SubSizer2;

	wxImage panelSenderImage;
	wxArrayString features;
	wxArrayString optionsLSS;
	wxArrayString optionsTD;

	wxStaticText* panelSenderTextTitle;
	wxRadioBox* panelSenderFeatures;
	wxRadioBox* panelSenderOptionsLSS;
	wxRadioBox* panelSenderOptionsTD;
	wxButton* panelSenderButtonConfirm;
	wxButton* panelSenderButtonExit;
	wxStaticBitmap* panelSenderImageDisplay;

	wxBoxSizer* panelSenderMainSizer;
	wxBoxSizer* panelSenderSubSizer1;
	wxBoxSizer* panelSenderSubSizer2;
	wxBoxSizer* panelSenderSubSizer3;
	wxBoxSizer* panelSenderSubSizer4;

	wxStaticText* panelAuthorizationTextTitle;
	wxStaticText* panelAuthorizationTextDescription;
	wxStaticText* panelAuthorizationTextAsk;
	wxButton* panelAuthorizationButtonYes;
	wxButton* panelAuthorizationButtonNo;
	wxButton* panelAuthorizationButtonConfirm;
	wxButton* panelAuthorizationButtonExit;
	wxStaticText* panelAuthorizationTextAuthorization;
	wxStaticText* panelAuthorizationTextSuccess;
	wxStaticText* panelAuthorizationTextFail;
	wxTextCtrl* panelAuthorizationInputField;
	wxTimer* panelAuthorizationTimer;

	wxBoxSizer* panelAuthorizationMainSizer;
	wxBoxSizer* panelAuthorizationSubSizer1;
	wxBoxSizer* panelAuthorizationSubSizer2;
	wxBoxSizer* panelAuthorizationSubSizer3;

	wxStaticText* panelReceiverTextTitle;
	wxButton* panelReceiverButtonExit;
	wxStaticBitmap* panelReceiverImageDisplay;
	wxStaticText* panelReceiverTextWaiting;
	wxStaticText* panelReceiverTextProcessing;
	wxStaticText* panelReceiverTextFeature;
	wxTimer* panelReceiverTimer;

	wxBoxSizer* panelReceiverMainSizer;
	wxBoxSizer* panelReceiverSubSizer1;

	wxBoxSizer* sizerMain;

	//Socket
	std::string ip_address;
	int port;

	std::string client_id = "YOUR_CLIENT_ID";
	std::string client_secret = "YOUR_CLIENT_SECRET";
	std::string redirect_uri = "YOUR_REDIRECT_URI";
	std::string access_token;
	std::string refresh_token;
	std::string authorization_code;
	wxScopedPtr<Client> client;
	wxScopedPtr<GmailClient> gmailClient;
};