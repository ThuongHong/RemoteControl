#pragma once
#include <wx/wx.h>
#include <wx/timer.h>

class RemoteControlDraft : public wxFrame
{
public:
	RemoteControlDraft(const wxString& title);
private:
	void CreatePanel1(wxString description, wxImage image);
	void SetPanel1(wxFont headerFont, wxFont mainFont, wxBitmap bitmap);
	void CreateSizerPanel1();

	void CreatePanel2(wxString description, wxImage image);
	void SetPanel2(wxFont headerFont, wxFont mainFont);
	void CreateSizerPanel2();

	void CreatePanel3(wxImage image);
	void SetPanel3(wxFont headerFont, wxFont mainFont);
	void CreateSizerPanel3();

	void CreatePanel4();
	void SetPanel4(wxFont headerFont, wxFont mainFont);
	void CreateSizerPanel4();

	void BindControl();

	void OnPanel1ButtonClicked(wxCommandEvent& evt);
	void OnPanel2ButtonClicked(wxCommandEvent& evt);
	void OnPanel2RolesChanged(wxCommandEvent& evt);
	void OnPanel2TextCtrlChanged(wxCommandEvent& event);
	bool IsEmailFormat(const wxString& text);
	bool IsIPFormat(const wxString& text);
	void OnPanel3FeaturesChanged(wxCommandEvent& evt);
	void OnPanel3ButtonConfirm(wxCommandEvent& evt);
	void OnButtonExitClicked(wxCommandEvent& evt);

	bool OnPanel4EventListened();
	void OnTimer(wxTimerEvent& event);
	void UpdateStatusText();

	wxPanel* panel1;
	wxPanel* panel2;
	wxPanel* panel3;
	wxPanel* panel4;
	wxPanel* panel5;

	wxString description;

	wxStaticText* panel1ProgramTitle;
	wxStaticText* panel1ProgramDescription;
	wxButton* panel1ButtonLogin;
	wxImage panel1Image;
	wxStaticBitmap* panel1Logo;
	wxArrayString roles;

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
	wxRadioBox* panel3OptionsLSS;
	wxRadioBox* panel3OptionsTD;
	wxButton* panel3ButtonConfirm;
	wxButton* panel3ButtonExit;
	wxStaticBitmap* panel3ImageDisplay;

	wxBoxSizer* panel3MainSizer;
	wxBoxSizer* panel3SubSizer1;
	wxBoxSizer* panel3SubSizer2;
	wxBoxSizer* panel3SubSizer3;
	wxBoxSizer* panel3SubSizer4;

	wxStaticText* panel4Receiver;
	wxButton* panel4ButtonExit;
	wxStaticBitmap* panel4ImageDisplay;
	wxStaticText* panel4TextWaiting;
	wxStaticText* panel4TextProcessing;
	wxStaticText* panel4TextFeature;
	wxTimer* panel4Timer;

	wxBoxSizer* panel4MainSizer;
	wxBoxSizer* panel4SubSizer1;

	wxBoxSizer* sizerMain;

};

