#ifndef ___CHAT__DIM___
#define ___CHAT__DIM___
#define TEMP_IP "98.208.20.5"
#include <mxdimension.h>
#include "console.h"
#include<boost/thread.hpp>
#include<boost/scoped_ptr.hpp>
#include<sys/socket.h>
#include<sys/types.h>
#include<arpa/inet.h>
#include<sys/poll.h>



namespace masterx {


	using namespace mx;
	using namespace mxui;


	class chatSocketConnection {


	public:
		chatSocketConnection() { sockfd = 0; }
		~chatSocketConnection() { if(sockfd != 0) close(sockfd); }
		chatSocketConnection(string ip);
		void connectTo(string ip);
		bool connectComplete(string ip);
		bool connect();
		string recv();
		int send(string);
		void closeConnection();

		const int getsockfd() const { return sockfd; }

	protected:
		int sockfd;
		struct sockaddr_in  addy;


	};

	class socketListener;

	class  chatDimension : public Dimension  {

		friend class socketListener;

	public:
		chatDimension(mx::mxWnd *_handle) : handle(_handle)
		{
			setName("MasterX - Online Chat");
			setBackground("img/chatbg.png");

			login_window = new Window();
			login_window->createWindow(Color(254,254,254), "Login", Point(300, 30), Size(380, 480), Color(255,0,0), Color(0,0,0), Color(0,0,255));
			login_window->useSkin("img/skins/mx/blue");
			login_window->setPos( Point(300,50) );
			login_window->setAlpha(0.2f);
			login_window->setAlphaBlend(this, true);
			login_window->canClose(false);
			login_window->canMax(false);


			addWindow(login_window);

			login_window->show();

			Label *label = new Label(login_window, "", Point(0,0));
			label->setDrawSurface(  login_window->Surface() );
			label->setPos( Point( 25, 69) );
			label->setText(Color(0,0,0), "Handle: ");

			login_window->addWidget(label);

			handle_edit = new LineEdit(login_window, Point(95,65), 150);

			login_window->addWidget(handle_edit);

			handle_edit->setFocus(true);
			login_window->setFocus(handle_edit);


			sign_in = new ActionButton<chatDimension>(login_window);
			sign_in->setConnector(this, &chatDimension::action);
			sign_in->createButton(Rect(262, 62, 100, 30), "Sign in", Color(150,150,150), Color(200,200,200), Color(0,0,0));

			login_window->addWidget(sign_in);

			connect_status = new Label(login_window, "", Point(0,0));
			connect_status->setDrawSurface( login_window->Surface() );
			connect_status->setPos(Point(25, 95));
			connect_status->setText(Color(255,255,255), "Connection Status: Waiting for user..");

			login_window->addWidget(connect_status);


			img = new Image(login_window, "img/chatmsg.png", Point(25,200));
			img->setPos(Point(20,140));
			login_window->addWidget(img);


			chat_window = new Window();
			chat_window->createWindow(Color(0,0,0), "MasterX Chat Room", Point(0, 25), Size(1024, 768-25), Color(255,0,0), Color(0,0,0), Color(0,0,0));
			chat_window->setPos( Point(0,25) );
			chat_window->setSize( Size(1024,768-25) );
			chat_window->maximizeWindow();
			chat_window->useSkin("img/skins/mx/black");
			chat_window->setAlpha(0.9f);
			chat_window->setAlphaBlend(this, true);
			chat_window->canMax(false);
			chat_window->canClose(false);


			addWindow(chat_window);

			chat_window->hide();

			area = new DisplayArea(chat_window);
			area->setPos( Point(25, 25) );

			area->setSize( Size(1000,700));
			area->setColor( Color(255,0,0) );
			area->concat(" Welcome to MasterX Online...\n");
			area->setDrawingSurface( chat_window->Surface() );
			area->setColor(Color(255,0,0));

			edit_line = new ActionLine<chatDimension>( chat_window, Point(25, 768-75), 824 );
			edit_line->setConnector(this, &chatDimension::action);

			send_bt =  new ActionButton<chatDimension>(chat_window);
			dis_bt = new ActionButton<chatDimension>(chat_window);

			send_bt->setConnector(this, &chatDimension::action);
			dis_bt->setConnector(this, &chatDimension::action);

			send_bt->createButton(Rect(824+25, 768-75, 73, 25), "Send", Color(150,150,150), Color(200,200,200), Color(0,0,0));
			dis_bt->createButton(Rect(825+25+73, 768-75, 73, 25), "Quit", Color(150,150,150), Color(200,200,200), Color(0,0,0));

			chat_window->addWidget(area);
			chat_window->addWidget(edit_line);
			chat_window->addWidget(send_bt);
			chat_window->addWidget(dis_bt);

		}

		int action(widgetAction &we) {

			ActionButton<chatDimension> *bt = (ActionButton<chatDimension>*)we.extra;


			if(bt->ident() == sign_in->ident() && handle_edit->getText().length()>3) {
				if(sign_in->text == "Sign in")
				Login();
				else
				Disconnect();
				return 0;
			}

			if(bt->ident() == dis_bt->ident()) {
				Disconnect();
				return 0;
			}

			if(bt->ident() == send_bt->ident()) {
				Send();
			}

			if(we.actionCode == MX_EDIT_ENTER) {
				Send();
			}

			return 0;
		}

		void setUserName(string name) { this->name = name; }
		string getUserName() { return name;  }
		void Login();
		void Disconnect();
		void Send();

	protected:
		mxWnd *handle;
		string name;
		chatSocketConnection online;
		Window *chat_window;
		Window *login_window;
		LineEdit *handle_edit;
		Label *connect_status;
		ActionButton<chatDimension> *sign_in, *send_bt, *dis_bt;
		Image *img;
		DisplayArea *area;
		ActionLine<chatDimension> *edit_line;
	};

	class socketListener   {
	public:
		socketListener(chatDimension *dim) {
			this->dim = dim;
		}

		void operator()() {

			bool active = true;

			struct pollfd  pfd;

			pfd.events = POLLIN;
			pfd.fd = (int)dim->online.getsockfd();



			while(active) {

				if(poll(&pfd, 1, 0)  < 0) {
					perror(" on poll");
				}

				if(pfd.revents) {

					try {

					std::string temp = dim->online.recv();
					dim->area->concat(temp+"\n");

					}
					catch( mx::mxException<std::string> &e) {

						e.printError(std::cerr);
						return;
					}

				}

			}


		}

	protected:
		chatDimension *dim;
		friend class chatDimension;
	};
}


#endif



