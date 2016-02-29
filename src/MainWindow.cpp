/*
 * MainWindow.cpp
 *
 *  Created on: Feb 20, 2016
 *      Author: zaqc
 */

#include <gtkmm.h>

#include "MainWindow.h"
#include "ScopeView.h"
#include "ScopeControl.h"
//----------------------------------------------------------------------------

//============================================================================
//	SliderRect
//============================================================================
SliderRect::SliderRect(float aMin, float aMax, float aMinVal, float aMaxVal, std::string aCaption, std::string aExt) :
		Gtk::Box(Gtk::ORIENTATION_VERTICAL, 4) {

	m_Min = aMin;
	m_Max = aMax;
	m_MinVal = aMinVal;
	m_MaxVal = aMaxVal;

	set_border_width(4);

	m_Caption = Gtk::manage(new Gtk::Label());
	m_Caption->set_text(aCaption);
	add(*m_Caption);

	m_Scale = Gtk::manage(new Gtk::Scale(Gtk::ORIENTATION_VERTICAL));
	m_Scale->set_adjustment(Gtk::Adjustment::create(aMin, aMin, aMax));
	m_Scale->set_inverted(true);
	m_Scale->set_margin_left(2);
	m_Scale->set_margin_top(2);
	m_Scale->set_margin_bottom(2);
	m_Scale->set_margin_right(2);
	m_Scale->set_vexpand(true);
	m_Scale->set_draw_value(false);
	add(*m_Scale);

	m_Ext = aExt;
	m_Value = Gtk::manage(new Gtk::Label());
	m_Value->set_text("0" + aExt);
	m_Value->set_size_request(64, 0);
	add(*m_Value);
}
//----------------------------------------------------------------------------

SliderRect::~SliderRect() {

}
//----------------------------------------------------------------------------

void SliderRect::UpdateValue(double aVal) {
	char str[16];

	aVal = aVal < m_Min ? m_Min : aVal;
	aVal = aVal > m_Max ? m_Max : aVal;
	double v = m_MinVal + (aVal - m_Min) / (m_Max - m_Min) * (m_MaxVal - m_MinVal);

	sprintf(str, "%.1f%s", (float) v, m_Ext.c_str());
	m_Value->set_text(str);
}
//----------------------------------------------------------------------------

//============================================================================
//	MainWindow
//============================================================================
bool MainWindow::amp_one_change(Gtk::ScrollType type, double value) {

	if (value < 0) {
		m_AmpOne->m_Scale->set_value(0);
		return false;
	}

	m_AmpTwo->m_Scale->set_value(value + m_AmpDelta);

	m_AmpOne->UpdateValue(value);
	SetVRC();
	return true;
}
//----------------------------------------------------------------------------

bool MainWindow::amp_two_change(Gtk::ScrollType type, double value) {

	if (value < 0) {
		m_AmpTwo->m_Scale->set_value(0);
		return false;
	}

	if (value < m_AmpOne->m_Scale->get_value()) {
		m_AmpTwo->m_Scale->set_value(m_AmpOne->m_Scale->get_value());
		return false;
	} else {
		m_AmpDelta = value - m_AmpOne->m_Scale->get_value();
	}
	m_AmpTwo->UpdateValue(value);
	SetVRC();
	return true;
}
//----------------------------------------------------------------------------

bool MainWindow::vrc_len_change(Gtk::ScrollType type, double value) {

	m_VRCLen->UpdateValue(value);
	SetVRC();
	return true;
}
//----------------------------------------------------------------------------

void MainWindow::on_spin_button(void) {
	int val = (int) m_SpinButtonChNum->get_value();
	m_CurrentChannel = val - 1;
	if (m_CurrentChannel < 0)
		m_CurrentChannel = 0;
	if (m_CurrentChannel > 7)
		m_CurrentChannel = 7;
}
//----------------------------------------------------------------------------

void MainWindow::SetVRC(void) {

	m_ScopeControl->CMD_SetVRC(0, m_CurrentChannel, m_AmpOne->m_Scale->get_value(), m_AmpTwo->m_Scale->get_value(),
			m_VRCLen->m_Scale->get_value());

	m_ScopeView->SetVRC(m_AmpOne->m_Scale->get_value(), m_AmpTwo->m_Scale->get_value(), m_VRCLen->m_Scale->get_value());
}

MainWindow::MainWindow() {
	set_default_size(400, 200);
	set_title("PPTune");

	m_CurrentChannel = 0;

	m_ScopeControl = new ScopeControl();
	m_ScopeControl->InitCtrlSocket();
	m_ScopeControl->InitReceiver();

	Gtk::Box *vbox = Gtk::manage(new Gtk::Box(Gtk::ORIENTATION_HORIZONTAL, 4));
	add(*vbox);

	Gtk::Box *v_ctrl = Gtk::manage(new Gtk::Box(Gtk::ORIENTATION_VERTICAL, 4));
	v_ctrl->set_border_width(4);

	m_AmpDelta = 0;

	m_AmpOne = Gtk::manage(new SliderRect(0, 255, 0, 96, "У1", "дБ"));
	m_AmpOne->m_Scale->signal_change_value().connect(sigc::mem_fun(*this, &MainWindow::amp_one_change));
	vbox->add(*m_AmpOne);

	m_AmpTwo = Gtk::manage(new SliderRect(0, 255, 0, 96, "У2", "дБ"));
	m_AmpTwo->m_Scale->signal_change_value().connect(sigc::mem_fun(*this, &MainWindow::amp_two_change));
	vbox->add(*m_AmpTwo);

	m_VRCLen = Gtk::manage(new SliderRect(0, 70, 0, 180, "ВРЧ", "мкс"));
	m_VRCLen->m_Scale->signal_change_value().connect(sigc::mem_fun(*this, &MainWindow::vrc_len_change));
	vbox->add(*m_VRCLen);

	m_ScopeView = Gtk::manage(new ScopeView(m_ScopeControl));
	m_ScopeView->set_size_request(256, 128);
	m_ScopeView->set_vexpand(true);
	m_ScopeView->set_hexpand(true);
	vbox->add(*m_ScopeView);

	Gtk::MenuBar *menubar = Gtk::manage(new Gtk::MenuBar());
	vbox->pack_start(*menubar, Gtk::PACK_SHRINK, 0);

	Gtk::MenuItem *menuitem_file = Gtk::manage(new Gtk::MenuItem("_File", true));
	menubar->append(*menuitem_file);
	Gtk::Menu *filemenu = Gtk::manage(new Gtk::Menu());
	menuitem_file->set_submenu(*filemenu);
	Gtk::MenuItem *menuitem_quit = Gtk::manage(new Gtk::MenuItem("_Quit", true));
	menuitem_quit->signal_activate().connect(sigc::mem_fun(*this, &MainWindow::on_quit_click));

	filemenu->append(*menuitem_quit);

	Gtk::Grid *grid = Gtk::manage(new Gtk::Grid);
	grid->set_border_width(10);
	vbox->add(*grid);

	Gtk::Button *b1 = Gtk::manage(new Gtk::Button("Big Button 1"));
	//b1->set_hexpand (true);
	//b1->set_vexpand (true);
	b1->signal_clicked().connect(sigc::mem_fun(*this, &MainWindow::on_big_button1_click));
	grid->attach(*b1, 1, 0, 1, 2);

	Gtk::Button *b2 = Gtk::manage(new Gtk::Button("Button 2"));
	b2->signal_clicked().connect(sigc::mem_fun(*this, &MainWindow::on_button2_click));
	grid->attach(*b2, 2, 0, 1, 1);

	Gtk::Button *b3 = Gtk::manage(new Gtk::Button("Button 3"));
	b3->signal_clicked().connect(sigc::mem_fun(*this, &MainWindow::on_button3_click));
	grid->attach(*b3, 2, 1, 1, 1);

	Gtk::CheckButton *chb = Gtk::manage(new Gtk::CheckButton("Sync", false));
	grid->attach(*chb, 2, 2, 1, 1);

	m_SpinButtonChNum = Gtk::manage(new Gtk::SpinButton());
	m_SpinButtonChNum->set_adjustment(Gtk::Adjustment::create(1, 1, 8));
	m_SpinButtonChNum->update();
	m_SpinButtonChNum->signal_changed().connect(sigc::mem_fun(*this, &MainWindow::on_spin_button));
	grid->attach(*m_SpinButtonChNum, 2, 3, 1, 1);

	vbox->show_all();
}
//----------------------------------------------------------------------------

MainWindow::~MainWindow() {
	printf("destroy MainWindow \n");

	m_ScopeControl->DoneReceiver();
	delete m_ScopeControl;
}
//----------------------------------------------------------------------------

void MainWindow::on_big_button1_click() {
	dialog("Big Button 1 Pressed!");
}
//----------------------------------------------------------------------------

void MainWindow::on_button2_click() {
	dialog("Button 2 Pressed!");
}
//----------------------------------------------------------------------------

void MainWindow::on_button3_click() {
	dialog("Button 3 Pressed!");
}
//----------------------------------------------------------------------------

void MainWindow::on_quit_click() {
	hide();
}
//----------------------------------------------------------------------------

void MainWindow::dialog(Glib::ustring msg) {
	Gtk::MessageDialog dlg(msg, false, Gtk::MESSAGE_INFO, Gtk::BUTTONS_OK, true);
	dlg.set_title("Gtkmm Tutorial 3");
	dlg.run();
}
//----------------------------------------------------------------------------
