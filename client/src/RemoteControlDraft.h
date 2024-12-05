#pragma once
#include <wx/wx.h>
#include <wx/timer.h>
#include <memory>
#include "client.h"
#include "client_socket.h"
#include "panels.h"

class RemoteControlDraft : public wxFrame
{
public:
	RemoteControlDraft(const wxString& title);
private:
	//GUI
	PanelLogin* panelLogin;
	PanelRoles* panelRoles;
	PanelAuthorization* panelAuthorization;
	PanelSender* panelSender;
	PanelReceiver* panelReceiver;

	wxString description;
	wxBoxSizer* sizerMain;
	wxStaticText* m_statusText;

	//Socket
	std::string ip_address;
	int port;

	std::string client_id = "YOUR_CLIENT_ID";
	std::string client_secret = "YOUR_CLIENT_SECRET";
	std::string redirect_uri = "YOUR_REDIRECT_URI";
	std::string target_email;
	std::string access_token;
	std::string refresh_token;
	std::string authorization_code;
	wxScopedPtr<Client> client;
	wxScopedPtr<GmailClient> gmailClient;
};