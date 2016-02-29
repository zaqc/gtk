//============================================================================
// Name        : tts.cpp
// Author      : zaqc
// Version     :
// Copyright   : Your copyright notice
// Description : Hello World in C++, Ansi-style
//============================================================================

#include <gtkmm.h>

#include "MainWindow.h"

int main(int argc, char **argv) {
	Glib::RefPtr<Gtk::Application> app = Gtk::Application::create(argc, argv,
			"com.gtkmm.tutorial3.base");

	MainWindow window;
	window.set_default_size(1024, 768);
	//window.move(650, 250);
	window.set_position(Gtk::WIN_POS_CENTER);

	return app->run(window);
}
