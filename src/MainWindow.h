/*
 * MainWindow.h
 *
 *  Created on: Feb 20, 2016
 *      Author: zaqc
 */

#ifndef MAINWINDOW_H_
#define MAINWINDOW_H_

#include <gtkmm.h>

#include <string>

#include "ScopeView.h"

class ScopeControl;
class MainWindow;

class SliderRect: public Gtk::Box {
public:
	Gtk::Label *m_Caption;
	Gtk::Scale *m_Scale;
	Gtk::Label *m_Value;

	std::string m_Ext;

	float m_Min;
	float m_Max;
	float m_MinVal;
	float m_MaxVal;

	SliderRect(float aMin, float aMax, float aMinVal, float aMaxVal, std::string aCaption, std::string aExt);
	virtual ~SliderRect();

	void UpdateValue(double aVal);
	void SetValue(double aVal);
};

class MainWindow: public Gtk::Window {
public:
	MainWindow();
	virtual ~MainWindow();

private:
	int m_AmpDelta;
	SliderRect *m_AmpOne;
	SliderRect *m_AmpTwo;
	SliderRect *m_VRCLen;

	int m_CurrentChannel;
	Gtk::SpinButton *m_SpinButtonChNum;

	ScopeControl *m_ScopeControl;
	ScopeView *m_ScopeView;

	bool amp_one_change(Gtk::ScrollType type, double value);
	bool amp_two_change(Gtk::ScrollType type, double value);
	bool vrc_len_change(Gtk::ScrollType type, double value);

	void on_spin_button(void);

	void SetVRC(void);

	void on_big_button1_click();
	void on_button2_click();
	void on_button3_click();
	void on_quit_click();
	void dialog(Glib::ustring msg);
};

#endif /* MAINWINDOW_H_ */
