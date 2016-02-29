/*
 * ScopeView.cpp
 *
 *  Created on: Feb 20, 2016
 *      Author: zaqc
 */

#include <gtkmm.h>
#include <gdkmm.h>
#include <cairomm/context.h>
#include <glibmm/main.h>

#include "ScopeView.h"
#include "ScopeControl.h"

void * update_scope_view_thread_proc(void *arg) {
	((ScopeView*) arg)->UpdateThread();
	return NULL;
}

ScopeView::ScopeView(ScopeControl *aScopeControl) {
	// TODO Auto-generated constructor stub

	m_ScopeControl = aScopeControl;

	pthread_mutex_init(&m_Lock, NULL);
	pthread_cond_init(&m_Ready, NULL);

	m_UpdateRunning = false;
	pthread_mutex_lock(&m_Lock);
	m_UpdateRunning = pthread_create(&m_UpdateThread, NULL, &update_scope_view_thread_proc, this) == 0;
	pthread_mutex_unlock(&m_Lock);

	Glib::signal_timeout().connect(sigc::mem_fun(*this, &ScopeView::on_timeout), 5);
}

ScopeView::~ScopeView() {
	printf("destroy ScopeView \n");

	bool running = false;
	pthread_mutex_lock(&m_Lock);
	running = m_UpdateRunning;
	m_UpdateRunning = false;
	pthread_cond_signal(&m_Ready);
	pthread_mutex_unlock(&m_Lock);

	if (running) {
		pthread_join(m_UpdateThread, NULL);
	}

	pthread_cond_destroy(&m_Ready);
	pthread_mutex_destroy(&m_Lock);
}

void ScopeView::UpdateThread(void) {

	while (true) {
		pthread_mutex_lock(&m_Lock);

		pthread_cond_wait(&m_Ready, &m_Lock);
		if (!m_UpdateRunning) {
			pthread_mutex_unlock(&m_Lock);
			break;
		}

		m_DataSize = m_ScopeControl->GetRecvData(m_Data, 0x4000);

		usleep(5000);

		pthread_mutex_unlock(&m_Lock);

	}
}

double shift = 0.0;
int qp = 0;
bool ScopeView::on_draw(const Cairo::RefPtr<Cairo::Context>& cr) {

	int h = get_allocated_height();
	int w = get_allocated_width();

	if (h == 0 || w == 0)
		return false;

	pthread_mutex_lock(&m_Lock);

	if (cr) {
		cr->set_source_rgb(1.0, 1.0, 1.0);

		cr->rectangle(0, 0, w, h);
		cr->fill();

		cr->set_source_rgb(.25, .25, .25);

		cr->move_to(0, 0);
		cr->line_to(w, h);
		cr->move_to(0, h);
		cr->line_to(w, 0);

		cr->move_to(0, h / 2);
		for (int i = 0; i < w; i++) {
			int v = h / 2 - (unsigned char) m_Data[i];
			if (v >= h)
				v = h - 1;
			if (v < 0)
				v = 0;
			cr->line_to(i, v);
		}
//		for (int i = 0; i < w; i++) {
//			double v = sin((double) i / 100.0 + shift) * (h / 2);
//			cr->line_to(i, h / 2 + v);
//		}
//		shift += 0.1;

		int y = 0;
		cr->move_to(0, h / 3);
		for (int i = 0; i < 256; i++) {
			if (i < m_VL) {
				y = (float) m_A1 / (float) m_VL * (float) i;
			} else {
				y = m_A1 + (float) (m_A2 - m_A1) / (float) (256 - m_VL) * (float) (i - m_VL);
			}
			cr->line_to(i * 2, h / 3 - y);
		}

		cr->stroke();

		cr->move_to(100, 100);
		qp--;
		char str[20];
		sprintf(str, " %i  ", qp);
		cr->set_font_size(14.0);
		cr->show_text(std::string("assd assd assd") + std::string(str));

	} else {
		pthread_mutex_unlock(&m_Lock);
		return false;
	}

	pthread_cond_signal(&m_Ready);

	pthread_mutex_unlock(&m_Lock);

	return true;
}

bool ScopeView::on_timeout(void) {
	if (qp < 1) {
		qp++;
		queue_draw();
	}

	return true;
}

