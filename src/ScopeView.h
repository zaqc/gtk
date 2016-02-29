/*
 * ScopeView.h
 *
 *  Created on: Feb 20, 2016
 *      Author: zaqc
 */

#ifndef SCOPEVIEW_H_
#define SCOPEVIEW_H_

#include <gtkmm/drawingarea.h>

class ScopeControl;

class ScopeView: public Gtk::DrawingArea {
protected:
	pthread_mutex_t m_Lock;
	pthread_cond_t m_Ready;
	pthread_cond_t m_DataReady;

	ScopeControl *m_ScopeControl;

	bool m_UpdateRunning;
	pthread_t m_UpdateThread;

	int m_DataSize;
	char m_Data[0x4000];

	int m_A1, m_A2, m_VL;

	bool on_draw(const Cairo::RefPtr<Cairo::Context>& cr);
	bool on_timeout(void);

public:
	ScopeView(ScopeControl *aScopeControl);
	virtual ~ScopeView();

	void SetVRC(int aA1, int aA2, int aVL){
		m_A1 = aA1;
		m_A2 = aA2;
		m_VL = aVL;
	}

	void UpdateThread(void);
};

#endif /* SCOPEVIEW_H_ */
