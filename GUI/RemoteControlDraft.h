#pragma once
#include <wx/wx.h>

class RemoteControlDraft : public wxFrame
{
public:
	RemoteControlDraft(const wxString& title);
private:
	void OnPanel1ButtonSwitch(wxCommandEvent& evt);
	void OnPanel2ButtonSwitch(wxCommandEvent& evt);
	void OnPanel2RadioBoxChanged(wxCommandEvent& evt);
	void OnPanel2TextCtrlChanged(wxCommandEvent& event);
	bool IsEmailFormat(const wxString& text);
	bool IsIPFormat(const wxString& text);
	void OnPanel3FeaturesChanged(wxCommandEvent& evt);

	wxPanel* panel1;
	wxPanel* panel2;
	wxPanel* panel3;

	wxStaticText* panel1ProgramTitle;
	wxStaticText* panel1ProgramDescription;
	wxButton* panel1ButtonLogin;
	wxImage panel1Image;
	wxStaticBitmap* panel1Logo;

	wxBoxSizer* panel1MainSizer;
	wxBoxSizer* panel1SubSizer1;
	wxBoxSizer* panel1SubSizer2;


	wxStaticText* panel2ProgramTitle;
	wxStaticText* panel2ProgramDescription;
	wxStaticText* panel2SubText;
	wxRadioBox* panel2Roles;
	wxTextCtrl* panel2InputField;
	wxButton* panel2ButtonConfirm;

	wxBoxSizer* panel2MainSizer;
	wxBoxSizer* panel2SubSizer1;
	wxBoxSizer* panel2SubSizer2;

	wxImage panel3Image;
	wxArrayString features;
	wxArrayString optionsLSS;
	wxArrayString optionsTD;

	wxStaticText* panel3Sender;
	wxRadioBox* panel3Features;
	wxRadioBox* panel3Options;
	wxStaticText* panel3SubText;
	wxTextCtrl* panel3Directory;
	wxButton* panel3ButtonBrowse;
	wxButton* panel3ButtonConfirm;
	wxButton* panel3ButtonExit;
	wxStaticBitmap* panel3ImageDisplay;

	wxBoxSizer* panel3MainSizer = new wxBoxSizer(wxVERTICAL);
	wxBoxSizer* panel3SubSizer1 = new wxBoxSizer(wxHORIZONTAL);
	wxBoxSizer* panel3SubSizer2 = new wxBoxSizer(wxHORIZONTAL);
	wxBoxSizer* panel3SubSizer3 = new wxBoxSizer(wxVERTICAL);
	wxBoxSizer* panel3SubSizer4 = new wxBoxSizer(wxVERTICAL);
	wxBoxSizer* panel3SubSizer5 = new wxBoxSizer(wxHORIZONTAL);
	wxBoxSizer* panel3SubSizer6 = new wxBoxSizer(wxHORIZONTAL);


	wxBoxSizer* sizerMain;
};

