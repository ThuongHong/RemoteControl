#pragma once
#include <wx/wx.h>
#include <wx/timer.h>
#include <wx/webview.h>
#include <memory>
#include "client.h"
#include "panels.h"
#include "gmail.h"

class RemoteControlFrame : public wxFrame
{
public:
	RemoteControlFrame(const wxString &title);

private:
	// GUI
	PanelLogin *panelLogin;
	PanelRoles *panelRoles;
	PanelSender *panelSender;
	PanelReceiver *panelReceiver;
	PanelExplorer *panelExplorer;
	wxStaticText *m_statusText;
	wxWebView *webView;

	// Socket
	std::string ip_address;
	int port;

	// Gmail
	std::string client_id = "926757990224-84lbea6uthpg9kjodd8i9050gr5ie5gl.apps.googleusercontent.com";
	std::string client_secret = "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";
	std::string redirect_uri = "http://localhost:8080";
	std::string receive_email;
	std::string send_email;
	std::string access_token;
	std::string refresh_token;
	std::string authorization_code;

	// Gmail content
	std::string file_name;
	std::string app_svc_name;
	int processID;
	std::vector<std::string> tasks;

	// Classes
	wxScopedPtr<Client> client;
	wxScopedPtr<OAuth2Handler> oAuth2Handler;
	wxScopedPtr<GmailSender> gmailSender;
	wxScopedPtr<GmailReceiver> gmailReceiver;

	// Function
	void BindControl(wxScopedPtr<Client>& client);
	bool CreateOAuth2Handler(const std::string &client_id, const std::string &client_secret, const std::string &redirect_uri, wxScopedPtr<OAuth2Handler> &oAuth2Handler);
};