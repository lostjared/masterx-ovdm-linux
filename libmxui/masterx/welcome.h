#ifndef _WEL_
#define _WEL_
#include<mx.h>
#include<mxserver.h>
#include<mxl.h>
#include<vector>
#include<mxwidget_type.h>
#include"menu.h"
#include<mxl.h>
#include<fstream>
#include<mxsocket.h>
#include"chatdim.h"


namespace masterx {

	using namespace mxui;
	using namespace mx;
	using mxui::Dimension;

	class Welcome : public Dimension {

	public:
		Welcome(mxWnd *wnd, Size s);
		virtual ~Welcome() {

			FILE *f = fopen(".prefs", "r");
			if(f == 0) {

				mxlParser p;
				p.create_tag("prefs");

				//p.getTags()[name].tag["phone"] = phone;
				p.getTags()["prefs"].tag["online"] = (box1->getCheck() == true) ? "1" : "0";
				p.getTags()["prefs"].tag["python"] = (box2->getCheck() == true) ? "1" : "0";
				p.getTags()["prefs"].tag["menu"] =   (box3->getCheck() == true) ? "1" : "0";
				p.getTags()["prefs"].tag["trans"] = (box4->getCheck() == true) ? "1" : "0";

				 std::fstream file;
				 file.open(".prefs", std::ios::out);
				 p.printMXL_byIndex(file);
				 file.close();

			} else {
				fclose(f);
				mxlParser p;
				p.parseMXL_file(".prefs");
				p.getTags()["prefs"].tag["online"] = (box1->getCheck() == true) ? "1" : "0";
				p.getTags()["prefs"].tag["python"] = (box2->getCheck() == true) ? "1" : "0";
				p.getTags()["prefs"].tag["menu"] = (box3->getCheck() == true) ? "1" : "0";
				p.getTags()["prefs"].tag["trans"] = (box4->getCheck() == true) ? "1" : "0";
				std::fstream file;
				file.open(".prefs", std::ios::out);
				p.printMXL_byIndex(file);
				file.close();

			}


		}

		virtual int  mouseDown(int x,  int y, int button);
		virtual void  mouseUp(int x, int y,  int   button);


		void showAbout();
		void showPrefs();

		CheckBox *box4;
		CheckBox *box3;
		Window *win, *win2, *places;
	protected:
		Size size;
		mxWnd *p_wnd;
	private:

		int win1_id, win2_id;
		DimensionDock *dock;
		Window *prefs;
		CheckBox *box1;
		CheckBox *box2;
		Link *link;
		Link *clink;
		Link *dlink,*alink,*loglink;
		Window *status;
		Label *status_label;
		Link *status_link, *chat_link;

		class ProjectStatusDownloader {


		public:
			Welcome *welcome;

			ProjectStatusDownloader(Welcome *wel) : welcome(wel) {

				sockfd.createSocket();
			}

			void operator()() {

				if ( welcome->box1->getCheck() == false) {
					welcome->status_label->setText(Color(255,255,255), "You must first turn on, online settings\n in perfecenes");
					welcome->status->show();
					return;
				}

				static char buffer[1024];
				memset(buffer, 0, sizeof(buffer));
				sprintf(buffer, "%s", "GET /mxdata.php HTTP/1.0\n\n");
				if( sockfd.connectTo("205.234.202.125", 80) == false )
				{
					welcome->status_label->setText(Color(255,0,0), "Could not Connect");
					welcome->status->show();
					return;
				}
				if( sockfd.Write(buffer, strlen(buffer))  < 0 )  {
					return;
				}
				memset(buffer, 0, sizeof(buffer));
				signed int len_bytes=0;
				len_bytes =  sockfd.Read(buffer, sizeof(buffer));
				sockfd.closeSocket();

				if(len_bytes <= 0) {
					return;
				}

				buffer[len_bytes] = 0;
				std::string text=buffer;
				std::string str="UTF-8";
				text=text.substr(text.find(str)+str.length()+4, text.length());
				welcome->status_label->setText(Color(255,255,255), text.c_str());
				welcome->status->show();
			}

		protected:
			mxSocket sockfd;
			friend class Welcome;

		};

		friend class ProjectStatusDownloader;
	};

}


#endif


