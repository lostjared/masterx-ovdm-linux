#include"welcome.h"
#include<mxwidget_type.h>
#include"mxassert.h"
#include<boost/thread.hpp>
#include"console.h"
#include "masterx.h"

namespace masterx {


	using namespace  mx;
	using namespace  mxui;


	void run_script(std::string str) {

       std::string fullpath="scripts/" +   str +   ".py";
		FILE *fptr = fopen(fullpath.c_str(), "r");
		PyRun_SimpleFile(fptr, fullpath.c_str());
		fclose(fptr);
	}


	Welcome::Welcome(mxWnd *wnd, Size s) : Dimension("-[ Home ]-",wnd, s), size(s) , p_wnd(wnd)  {

		setBackground("img/dimx.png");

		places = new Window();

		places->createWindow(Color(254,254,254), "Places", Point(700, 35), Size(200, 480), Color(255,0,0), Color(0,0,0), Color(0,0,255));
		places->useSkin("img/skins/mx/blue");
		places->setWindowState(NORMAL);
		places->setAlpha(0.9f);
		places->setAlphaBlend(this, true);

		addWindow(places);


		alink = new Link(places, "About MasterX");

		alink->setPos( Point( 35, 25) );
		alink->setDrawSurface( places->Surface() );

		places->addWidget(alink);


		link = new Link(places, "Preferences");


		link->setPos( Point( 35, 50) );
		link->setDrawSurface( places->Surface() );

		places->addWidget(link);

		clink = new Link(places, "System Console");



		clink->setPos( Point( 35, 75) );
		clink->setDrawSurface( places->Surface() );

		places->addWidget(clink);

		dlink = new Link(places, "File Browser");



		dlink->setPos( Point( 35, 100) );
		dlink->setDrawSurface( places->Surface() );

		places->addWidget(dlink);


		loglink = new Link(places, "Log Out");

		loglink->setPos( Point( 35, 450) );
		loglink->setDrawSurface( places->Surface() );

		places->addWidget(loglink);


		status_link = new Link(places, "Project Status");

		__assert__ ( status_link != 0 );
		status_link->setPos( Point( 35, 125) );
		status_link->setDrawSurface( places->Surface() );

		places->addWidget(status_link);

		chat_link = new Link(places, "Chat Online");
		chat_link->setPos( Point(35, 150));
		chat_link->setDrawSurface( places->Surface() );

		places->addWidget(chat_link);


		win  = new Window();

		win->createWindow(Color(254,254,254), "About", Point(300, 70), Size(400, 480), Color(255,0,0), Color(0,0,0), Color(0,0,255));
		win->useSkin("img/skins/mx/blue");
		win->setWindowState(NORMAL);
		win->setAlpha(0.8f);
		win->setAlphaBlend(this, true);


		win1_id = addWindow(win);
		Image *img = new Image();
		img->loadImage("img/about.png");
		img->setPos( Point(50,30) );
		win->addWidget(img);
		win->hide();

		Label *l = new Label(win, "", Point(25,340));

		l->setText(Color(255,255,255), "This  application is dedicated  to the people\n** I love most **\nmost of all those people who\nbelieved in me\nAll those endless code sessions\nand years in study, were\nnot in vain.\n");
		l->setDrawSurface(win->Surface());
		l->setFont("verdana");

		win->addWidget(l);

		dock = new mxui::DimensionDock (this,  &windows, p_wnd->size().width, 30,p_wnd->size().height, background );

		dock->initRun( run_script );

		addWindow(dock);
		setFocus(window(win1_id)->getID());

		win2 = new Window();

		win2->createWindow(Color(255,255,255), "Welcome", Point(25,25),   Size(440,500), Color(255,0,0), Color(0,255,0), Color(0,0,255));
		win2->useSkin("img/skins/mx/black");
		win2->show();
		win2->setPos(Point(25,75));
		win2->setWindowState(NORMAL);
		win2->canClose(false);
		win2_id = addWindow(win2);
		win2->setAlpha(0.8f);
		win2->setAlphaBlend(this, true);

		Image *picture = new Image(win2, "img/dim1gfx.png", Point(250,20));


		picture->setPos(Point(70, 20));


		Label *good =  new Label(win2  , "",   Point(0,250));


		win2->addWidget(picture);

		good->setPos ( Point( 40, 200)  );
		good->setDrawSurface(  win2->Surface() );
		good->setText(Color(0xff,0xff,0xff), "Welcome to the Home Dimension\n\n In this multi-dimensional world youll use tools\n\n to create new worlds\n\n to share  new worlds with others\n\n\nTry using  dimension shift hotkeys\n\nFor more information read the README file\n\nVisit us online for more info");

		win2->addWidget(good);


		Label   *site = new Label(win2, "", Point(0,0));


		site->setPos(Point(100, 440));
		site->setDrawSurface( win2->Surface() );
		site->setText(Color(0,0,255), "http://lostsidedead.com");

		win2->addWidget(site);

		prefs  = new Window();

		prefs->createWindow(Color(255,255,255), "Preferences", Point(25,25),   Size(350,350), Color(255,0,0), Color(0,255,0), Color(0,0,255));
		prefs->useSkin("img/skins/mx/blue");
		prefs->hide();
		prefs->setPos( Point(250, 250) );
		prefs->setWindowState(NORMAL);
		prefs->setAlpha(0.9f);
		prefs->setAlphaBlend(this, true);

		addWindow(prefs);


		box1  =   new CheckBox(prefs, Size(30, 25));

		box1->setRenderSurface(&prefs->Surface());
		box1->setPos(Point(25,25));
		prefs->addWidget(box1);

		Label *online = new Label(prefs, "", Point(0,0));

		online->setPos(Point(75,30));
		online->setDrawSurface(prefs->Surface());
		online->setText(Color(255,255,255), "Connect MasterX Online");
		prefs->addWidget(online);

		box2 = new CheckBox(prefs, Size(30,25));
		box2->setRenderSurface(&prefs->Surface());
		box2->setPos(Point(25,60));
		prefs->addWidget(box2);

		Label *py  = new Label(prefs, "", Point(0,0));

		py->setPos(Point(75,60));
		py->setDrawSurface(prefs->Surface());
		py->setText(Color(255,255,255), "Allow Python Scripts");
		prefs->addWidget(py);


		box3 = new CheckBox(prefs, Size(30,25));

		box3->setRenderSurface(&prefs->Surface());
		box3->setPos(Point(25,95));
		prefs->addWidget(box3);


		box4 = new CheckBox(prefs, Size(30,25));


		box4->setRenderSurface(&prefs->Surface());
		box4->setPos(Point(25,125));
		prefs->addWidget(box4);

		Label *menu_on = new Label(prefs, "", Point(0,0));

		menu_on->setPos(Point(75, 95));
		menu_on->setDrawSurface(prefs->Surface());
		menu_on->setText(Color(255,255,255), "Auto Hide Menu");
		prefs->addWidget(menu_on);



		Label *trans_on = new Label(prefs, "", Point(0,0));

		trans_on->setPos(Point(75, 130));
		trans_on->setDrawSurface(prefs->Surface());
		trans_on->setText(Color(255,255,255), "Enable Dimension Transitions");

		prefs->addWidget(trans_on);



		FILE *f = fopen(".prefs", "r");
		if(f == 0) {
			box3->setCheck(150);
			return;
		}
		fclose(f);

		mxlParser p;
		p.parseMXL_file(".prefs");
		if( p.getTags()["prefs"].tag["python"] == "1") {

			box2->setCheck(150);

		} else box2->setCheck(0);

		if( p.getTags()["prefs"].tag["online"] == "1") {
			box1->setCheck(150);
		} else box1->setCheck(0);

		if(p.getTags()["prefs"].tag["menu"] == "1") {
			box3->setCheck(150);
		} else box3->setCheck(0);


		if(p.getTags()["prefs"].tag["trans"] == "1") {
			box4->setCheck(150);
			mX::server->trans = true;

		} else {
			mX::server->trans = false;
			box4->setCheck(0);
		}

		status = new Window();

		status->createWindow(Color(255,255,255), "Development Status", Point(25,25),   Size(520,440), Color(255,0,0), Color(0,255,0), Color(0,0,255));
		status->setPos(Point(250,25));
		status->useSkin("img/skins/mx/blue");
		status->setWindowState(NORMAL);
		status->canMax(false);
		status->hide();
		status->setAlpha(0.9f);
		status->setAlphaBlend(this, true);

		addWindow(status);

		status_label = new Label(status, "status", Point(25, 25));


		status_label->setDrawSurface( status->Surface() );
		status_label->setText(Color(255,255,255), "Could not Connect");

		status->addWidget(status_label);

	}

	void Welcome::showAbout()
	{
		win->show();
		Dimension::setFocus(win->getID());

	}

	void Welcome::showPrefs()
	{
		prefs->show();
		Dimension::setFocus(prefs->getID());

	}

	void Welcome::mouseUp(int x,  int  y, int button) {

		Dimension::mouseUp(x,y,button);

		if(box4->getCheck() == true) mX::server->trans = true;
		else mX::server->trans = false;

		if(box1->getCheck() == true) chat_link->show();
		else chat_link->hide();

	}

	int  Welcome::mouseDown(int x,  int y, int button)
	{

		if( Dimension::mouseDown(x,y,button) == MSG_PROC ) {
			if(focus_window->getID() != places->getID()) return MSG_PROC;
		}

		if(places->Visible()) {

			if(alink->mouseDown(x,y,button) == MSG_CLICKED) {
				showAbout();
				return MSG_PROC;
			}


			if(link->mouseDown(x,y,button) == MSG_CLICKED) {

				prefs->show();
				Dimension::setFocus(prefs->getID());
				return MSG_PROC;
			}


			if(clink->mouseDown(x,y,button) == MSG_CLICKED) {

				mX::server->setCurDim(2);
				return MSG_PROC;
			}

			if(dlink->mouseDown(x,y,button) == MSG_CLICKED) {

				mX::server->setCurDim(3);
				return MSG_PROC;
			}

			if(loglink->mouseDown(x,y,button) == MSG_CLICKED) {


				mX::server->cur_dimX = 0;
				mX::server->lockChange();
				return MSG_PROC;
			}

			if(status_link->mouseDown(x,y,button) == MSG_CLICKED) {

				ProjectStatusDownloader download(this);
				boost::thread t(download);
				return MSG_PROC;
			}

			if(chat_link->mouseDown(x,y,button) == MSG_CLICKED) {

				// new chat dim
				masterX_server *srv;
				srv = dynamic_cast<masterX_server*>(mX::server);
				chatDimension *dim = new chatDimension((mxWnd*)srv);
				dim->destroy = true;
				srv->addDimension(dim);
				srv->setFocus(dim->getID());

			}

		}

		return MSG_IGNORED;
	}
}
