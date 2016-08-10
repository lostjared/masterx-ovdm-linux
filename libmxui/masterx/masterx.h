/*

		|--=]QQQQQQQQQQQQQQQQQQQVTQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQ
		Q. ]QQQQQQQQQQQQQQQQQQQQ. mQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQ
		Q. ]QQQQQQV??9QQQ@T??9QT` ?TQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQ
		Q. ]QQQQQ( jw "Q@`_gg,dw  awQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQ
		Q. ]QQQQF .QQ; ]k +$QgmQ. mQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQ
		Q. ]QQQQ[ :QQ; ]m   -?QQ. mQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQ
		Q. ]QQQW[ :QQ; ]Wgs.  3Q. mQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQ
		Q. ]QQQ:h  QQ` jE4WQ; jQ. m@$WQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQ
		!  -""":QL.?! jQk ??`_QQc ?'dQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQ
		QmQQQQmQQQQwwmQWQmgygQQQQgwmQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQ
		QQQQQQQQQQQQQQQQQQ!.aa,-4QB  dQQQQQc )WQQQQQQQQWQQQWQQQWQQQWQQ
		QQQQQQQQQQQQQQQQQF jWWQ/jQQa<QQQQQQf )WQQQQQQQQQQQQQQQQQQQQQQQ
		QQQQQQQQQQQQQQQQQf )WQQmmQ???$QW!-"t )WQ@?"!4QQQQQQQQQQQQQQQQQ
		QQQQQQQQQQQQQQQQQL   "Y$QQm; dQ[ <g, )W@`.mc ]QQQQQQQQQQQQQQQQ
		QQQQQQQQQQQQQQQQQQ,     4QQ; dW  jQL )W[ =Qk -QQQQQQQQQQQQQQQQ
		QQQQQQQQQQQQQQQQQQQma,  =QQ; dW  3Qk )W[ -""  WWQQQQQQWQQQWQQQ
		QQQQQQQQQQQQQQQQQ@4QQQm .QQ; dQ  3Qf )W[ =QQQWWQQQQQQQQQQQQQQQ
		QQQQQQQQQQQQQQQQWk-QWQW <QQ; dQc ]Q[ )Wk -QWE:QQQQQQQQQQQQQQQQ
		QQQQQQQQQQQQQQQQQm,-!!`_mQc__|QQ,_=c__]Qa "!=yQQQQQQQQQQQQQQQQ
		QQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQW""!QQQQQQQQQQQQQQQQQQQQW"""Q
		QQQQQQQQQQQQQQQQQWQQQQQQQQQQQQQQQQE .QQQQQQQQQQQQQQQQQQQQQm  Q
		QQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQE :QQQQQQQQQQQQQQQQQQQWQW  Q
		QQQQQQQQQQQQQQQQQQQQQQQQWQQQQQW"  ! :QQ@!^^?QW@"++"4QQQ!  ?  Q
		QQQQQQQQQQQQQQQQWQQQQQQQQQQQQQ[ =Qc .QW' mk )WE<QQ; $Qf _QL  Q
		QQQQQQQQQQQQQQQQQQQQQQQWQQQQQQ` ]Qk :QF .Qm  $WWBH[ ]Q' ]Wm  Q
		QQQQQQQQQQQQQQQQQQQQQQQQQQQQQQ. jQE :Qf  |+..dD`.a, ]Q; ]WW  Q
		QQQQQQQQQQQQQQQQQQQQQQQQQQQQQQ; ]QE .QL  QQQQQ[ jW[ ]Q( ]WB  Q
		QQQQQQQQQQQQQQQQQQQQQWQQQQQQQQL +W[ :QQ. $Q@.Q[ ]B' ]Qk -Wf  Q
		QQQQQQQQQQQQQQQQWQQQQQQQQQQQQQQa,_w_a<Qg,-"<yQm, _c_=jQw,_w_a_
		[                                                            ]
		[                                                            ]
		[                                                            ]
		[   MAsterX-ovDM  (code name haze)                           ]
		[                                                            ]
		[   svn co svn://lostsidedead.com/haze                       ]
		[                                                            ]
		[   http://lostsidedead.com/project.php  (offical page)      ]
		[                                                            ]
		[             License:  GPL                                  ]
		[____________________________________________________________]

 */
#ifndef   _MASTERX_H_
#define   _MASTERX_H_
#include<mx.h>
#include<mxserver.h>
#include<mxl.h>
#include<mxplugin.h>
#include<mxwidget_type.h>
#include<mxui/menu.h>
#include<mxdimension.h>
#include<vector>
#include<cassert>
#include"welcome.h"
#include"console.h"
#include"login.h"
#include"mxdb.h"
#include"mxassert.h"
#include<pwd.h>
#include<sys/types.h>
#include<mximage.h>
#include<sstream>

#define _m_ STATIC_MAIN
#define _d_ masterX_Driver::main

namespace masterx  {

	using namespace mx;
	using namespace mxui;

	class masterX_server;


	class LoadingDialog {
	public:

		LoadingDialog(mxWnd *srv) {

			font.loadFont("fonts/system.mxf");
			this->mx = srv;
			display();

		}

		void display() {

			font.printText(mx->mxrenderSurface(), 25,25,mx::Color(0xff,0xff,0xff).toInteger(),  "Loading....");
			SDL_UpdateRect(mx->renderSurface(), 0, 0, mx->renderSurface()->w, mx->renderSurface()->h);

		}
	protected:
		mxWnd *mx;
		mx::mxRegularFont font;
	};
    

	class masterX_server : public  mX  {

		mx::mxImage img;

	public:

		int width, height;

		std::string getHome() {

			uid_t  id;
			struct passwd *pw;
			id = getuid();
			pw = getpwuid(id);
			return pw->pw_dir;
		}

		masterX_server(Size s, unsigned int flags=0) : mX(s,flags), load_dialog((mxWnd*)this), login((mxWnd*)this, s), welcome((mxWnd*)this, s), program_menu(front) , sizetype(s), console((mxWnd*)this, s),browser((mxWnd*)this, s)
		{

			unlockChange();
			program_menu.parseMenu("img/menu/login.mxl");
			program_menu.SetLength(s.width);
			addDimension(&login);
			addDimension(&welcome);
			addDimension(&console);
			addDimension(&browser);
			cur_dimX =  0;
			setClose(true);
			dmenu=false;
			always_menu = false;
		}

		void setAlwaysMenu(bool menuX) {
			always_menu = menuX;
		}


		void changeDimension(int id) {

			cur_dimX =  id;
			SDL_WM_SetCaption(dimensions[cur_dimX]->getname().c_str(), 0);
		}

		virtual void eventPassed(SDL_Event &e) {

#define DMODE

#ifdef DMODE
			if(e.type == SDL_KEYDOWN && e.key.keysym.sym == SDLK_F2){
				static unsigned int cx = 0;
				string homedir = getHome();
				std::ostringstream ss;
				ss << homedir << "/capture " << ++cx << ".jpg";
				img.saveJPG(SDL_GetVideoSurface(), ss.str());
			}
#endif
			if(cur_dimX == 0) { mX::eventPassed(e); return; }
			unsigned char *keys = SDL_GetKeyState(0);

			if(e.type == SDL_KEYDOWN && e.key.keysym.sym == SDLK_ESCAPE && keys[SDLK_LSHIFT]) {
					quit(); return;
			}

			if(e.type == SDL_KEYDOWN && e.key.keysym.sym == SDLK_F1) { setCurDim(2); return; }

			unsigned char *keybuf = SDL_GetKeyState(0);

			if(e.type == SDL_KEYUP && keybuf[SDLK_LSHIFT] && e.key.keysym.sym == SDLK_LEFT) {


				if( ((cur_dimX-1) == 0)) return;
				shift_left();
			}


			if(e.type == SDL_KEYUP && keybuf[SDLK_LSHIFT] && e.key.keysym.sym == SDLK_RIGHT) {
				shift_right();
			}


			mX::eventPassed(e);
		}


		virtual int  mouseMotion(int  x, int y, int state) {

			if(x > 0 && x < front.width() && y > 0 && y < 25)
				dmenu = true;
			else
			{
				if(program_menu.show_menu == -1) dmenu = false;
			}

			program_menu.mouseMotion(x,y, state);

			return   0;
		}

		virtual int mouseDown(int x, int y, int button) {

			int value = 0;

			value = program_menu.mouseDown(x,y,button);

			if(value != -1) {

				switch(value) {
				case 1: quit(); break;
				case 2: {
					//  full screen
					mx::mxSurface &surf = mX::server->mxrenderSurface();
					surf  = SDL_SetVideoMode(width, height, 0, SDL_FULLSCREEN);
				}
					break;
				case 3: {
					//  windowed
					mx::mxSurface &surf = mX::server->mxrenderSurface();
					surf = SDL_SetVideoMode(width, height, 0, 0);
				}
					break;
				case 4:
                        cur_dimX = 0;
                        login.setLogin();
                        lockChange();
                        break;
                        
                case 5:
                        cur_dimX = 0;
                        login.changeUserPW();
                        lockChange();
                        break;
				case 57:
                        setCurDim(1);
                        break;
				case 80:
					setCurDim(2);
					console.con->setWindowState(NORMAL);
				break;
				case  20:
					setCurDim(1);
					welcome.showPrefs();
					break;

				case 2116:
				{
					std::string homedir = getHome() + "/capture.bmp";
					SDL_SaveBMP(mX::server->renderSurface(), homedir.c_str());

					break;
				}
				case 257: {


					unsigned int pos = 0;

					std::vector<mxui::Dimension *>::iterator i;

					for(i = dimensions.begin(); i != dimensions.end(); i++)  {

						mxui::Dimension *d = (*i);


						if(d->getname() == "Dimension Browser") {

							setCurDim( pos );
							return -1;
						}
						pos ++;
					}


				}
				break;
				case 6:
					setCurDim(1);
					welcome.showAbout();
					break;
				case 22:
					if((cur_dimX-1) == 0) return -1;
					shift_left();
				break;
				case 21:
					shift_right();
					break;
				}

			}

			return value;
		}

		virtual void renderScreen() {


		static float alpha =0.0;



		if(change_dim == true) {

			if(trans == false) {
				cur_dimX = dim_change;
				change_dim = false;
				alpha = 0.0f;
				return;
			}


			unsigned int dimX =  dim_change;
			unsigned int dimY =  cur_dimX;

			dimensions[dimX]->render();

			mxPainter  dim1(dimensions[dimX]->Surface());
			mxPainter  dim2(dimensions[dimY]->Surface());
			mxPainter  paintObj(front);

			dim1.lock();
			dim2.lock();
			paintObj.lock();

			mx::Color col1, col2, final;

			for(int i = 0; i < front.width(); i++)   {

				for(int z = 0; z < front.height(); z++)  {

					dim1.GetPixel(i,z, col1);
					dim2.GetPixel(i,z, col2);

					unsigned char r,g,b;

					r = static_cast<unsigned char>( alpha*col1.color.colors[0] + (1-alpha) * col2.color.colors[0] );
					g = static_cast<unsigned char>( alpha*col1.color.colors[1] + (1-alpha) * col2.color.colors[1] );
					b = static_cast<unsigned char>( alpha*col1.color.colors[2] + (1-alpha) * col2.color.colors[2] );
					final = mx::Color(r,g,b);
					paintObj.setpixel(i,z, final);
				}
			}

			alpha += 0.33f;

			dim1.unlock();
			dim2.unlock();
			paintObj.unlock();
			front.Flip();

			if(alpha >= 1.02) {
				cur_dimX =  dim_change;
				change_dim = false;
				alpha = 0.0f;
			}

			return;

		}




			Dimension *cur_dim = dimensions[cur_dimX];

			__assert__(cur_dim != 0);


			cur_dim->render();
			cur_dim->drawScreen(front);


			if(welcome.box3->getCheck() == true) always_menu = false; else always_menu = true;

			if((cur_dimX != 0  && dmenu == true) || (cur_dimX != 0 && always_menu == true)) program_menu.draw();

			front.Flip();

		}

		virtual void erase() {}

	protected:
		LoadingDialog  load_dialog;
		mxui::LoginDimension login;
		class Welcome   welcome;
        mxui::Menu program_menu;
		Size sizetype;
		consoleDim console;
		dimBrowser  browser;
		bool dmenu;
		bool always_menu;

	};


}


#endif




