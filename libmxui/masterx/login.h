#ifndef _LOGIN_
#define _LOGIN_

#include<mx.h>
#include<mxwindow.h>
#include<mxdimension.h>
#include"console.h"
#include<mxl.h>
#include"masterx.h"




namespace mxui {

using namespace mx;
using namespace masterx;


	class LoginDimension : public Dimension {
	public:
		LoginDimension(mxWnd *w, Size s) : Dimension("Login", w, s) {

			login_mode = 0;

			setBackground("img/loginbg.png");
			window = new Window();
			login_id = addWindow(window);
			window->createWindow(Color(254,254,254), "Enter Login Information", Point(0, 0), Size(450, 200), Color(255,255,255), Color(0,0,0), Color(0,0,255));
			window->setPos(Point(265,423));
			window->useSkin("img/skins/mx/blue");
			window->canClose(false);
			window->canMax(false);
			window->setAlpha(0.8f);
			window->setAlphaBlend(this, true);


			change = new Window();
			change->createWindow(Color(254,254,254), "Change User/Pass", Point(0, 0), Size(450, 280), Color(255,255,255), Color(0,0,0), Color(0,0,255));
			change->setPos( Point(200,300) );
			change->useSkin("img/skins/mx/blue");
			change->canClose(false);
			change->canMax(false);
			change->setAlpha(0.8f);
			change->setAlphaBlend(this, true);

			addWindow(change);


			Label *olduser  = new Label(change, "", Point(25,75));
			olduser->setText(Color(255,0,0), "Current User Name: ");
			olduser->setDrawSurface(change->Surface());
			change->addWidget(olduser);

			old_user = new LineEdit(change, Point(200, 75), 150);

			change->addWidget(old_user);

			Label *oldpw;

			oldpw = new Label(change, "", Point(25,110));
			oldpw->setText(Color(255,0,0), "Current Password: ");
			oldpw->setDrawSurface(change->Surface());

			change->addWidget(oldpw);

			old_password = new LineEdit(change,Point(200, 110), 150);
			change->addWidget(old_password);
			old_password->passwordOn();





			Label *newuser;

			newuser = new Label(change, "",  Point(25,145));
			newuser->setText(Color(255,255,255), "New User Name: ");
			newuser->setDrawSurface(change->Surface());

			change->addWidget(newuser);
			new_user = new LineEdit(change,Point(200,145), 150);
			change->addWidget(new_user);


			Label *newpw;

			newpw = new Label(change, "", Point(25, 180));
			newpw->setText(Color(255,255,255), "New Password: ");
			newpw->setDrawSurface(change->Surface());

			change->addWidget(newpw);

			new_password = new LineEdit(change,Point(200,180), 150);
			change->addWidget(new_password);
			new_password->passwordOn();



			Label *lbl =  new Label(window, "", Point(25, 75));
			lbl->setText(Color(255,255,255), "Login ID: ");
			lbl->setDrawSurface(window->Surface());
			window->addWidget(lbl);
			Label  *label = new Label(window, "", Point(25,110));
			label->setText(Color(255,255,255), "Password");
			label->setDrawSurface(window->Surface());
			window->addWidget(label);

			button = new ActionButton<LoginDimension>(window);

			button->setConnector(this, &LoginDimension::procEvent);
			button->createButton(Rect(310, 135, 100, 30), "Login", Color(150,150,150), Color(200,200,200), Color(0,0,0));

			change_upw = new ActionButton<LoginDimension>(change);
			change_upw->setConnector(this, &LoginDimension::procEvent);
			change_upw->createButton(Rect(310,220, 100,30), "Change", Color(150,150,150), Color(200,200,200), Color(0,0,0));

			change->addWidget(change_upw);


			window->addWidget(button);

			user = new LineEdit(window, Point(115, 70), 170);
			user_id = window->addWidget(user);

			pass = new ActionLine<LoginDimension>(window,  Point(115,110), 170); //new LineEdit(window, Point(115, 110),170);
			pass->setConnector(this, &LoginDimension::procEvent);
			pass_id = window->addWidget(pass);
			user->setFocus(true);
			pass->passwordOn();
			window->setFocus(user);
			mX::server->lockChange();
			con = 0;
			window->setKeyInput(true);

			old_user->setFocus(true);
			change->setFocus(old_user);
			change->setKeyInput(true);

			msgbox = new MessageBox("img/skins/mx/blue", Point(200, 300), Size(320, 240), "Incorrect Password", "Password Incorrect\n\n Try again", MSG_OK);
			cool_box = new MessageBox("img/skins/mx/blue", Point(200,300), Size(320,240), "Changed User/Password", "Username and Password\n\n have changed, please Login", MSG_OK);

			msgbox->setAlpha(0.8f);
			msgbox->setAlphaBlend(this, true);

			cool_box->setAlpha(0.8f);
			cool_box->setAlphaBlend(this, true);

			addWindow(msgbox);
			addWindow(cool_box);


			setFocus(window->getID());
			msgbox->hide();
			cool_box->hide();
			SDL_WM_SetCaption("Login to MasterX System", 0 );
			setLogin();

		}

		virtual int  mouseDown(int x,  int y, int   button)
		{
			int rt = 0;

			if( Dimension::mouseDown(x,y,button) == MSG_PROC )  {

				if(login_mode == 1 && focus_window->getID() != change->getID())
					return MSG_PROC;

				if(login_mode == 0 && focus_window->getID() != window->getID())
					return MSG_PROC;
			}

			if(login_mode == 0) {

				rt = user->mouseDown(x,y,button);

				if(rt == MSG_FOCUS) {
					user->setFocus(true);
					pass->setFocus(false);
					window->setFocus(user);
					window->setKeyInput(true);
					return MSG_PROC;
				}

				rt = pass->mouseDown(x,y,button);

				if(rt  == MSG_FOCUS) {
					pass->setFocus(true);
					window->setFocus(pass);
					user->setFocus(false);
					window->setKeyInput(true);
					return MSG_PROC;
				}

			}
			else {
				rt = old_user->mouseDown(x,y,button);

				if(rt == MSG_FOCUS) {

					old_user->setFocus(true);
					window->setFocus(old_user);

					old_password->setFocus(false);
					new_password->setFocus(false);
					new_user->setFocus(false);
					window->setKeyInput(true);
					return MSG_PROC;
				}



				rt = old_password->mouseDown(x,y,button);

				if(rt == MSG_FOCUS) {

					old_password->setFocus(true);
					window->setFocus(old_password);

					old_user->setFocus(false);
					new_password->setFocus(false);
					new_user->setFocus(false);
					window->setKeyInput(true);
					return MSG_PROC;
				}

				rt = new_user->mouseDown(x,y,button);

				if(rt == MSG_FOCUS) {

					new_user->setFocus(true);
					window->setFocus(new_user);

					old_user->setFocus(false);
					old_password->setFocus(false);
					new_password->setFocus(false);
					window->setKeyInput(true);
					return MSG_PROC;
				}

				rt = new_password->mouseDown(x,y,button);

				if(rt == MSG_FOCUS) {

					new_password->setFocus(true);
					window->setFocus(new_password);

					old_user->setFocus(false);
					old_password->setFocus(false);
					new_user->setFocus(false);
					window->setKeyInput(true);
					return MSG_PROC;
				}

			}

			old_user->setFocus(false);
			new_user->setFocus(false);
			old_password->setFocus(false);
			new_password->setFocus(false);
			window->setFocus(old_user);
			window->setKeyInput(false);

			return MSG_IGNORED;

		}

		void changeUserPW() {

			login_mode = 1;
			change->show();
			window->hide();
			old_user->setText("");
			old_password->setText("");
			new_user->setText("");
			new_password->setText("");
		}

		void setLogin() {
			login_mode = 0;
			window->show();
			change->hide();
			user->setText("");
			pass->setText("");
		}

		void changePW() {


			if(old_user->getLength() == 0 || old_password->getLength() == 0 || new_user->getLength() == 0 || new_password->getLength() == 0) {
				std::cout << "error fill out all fields\n";
				return;
			}


			mxlParser p;

			if( p.parseMXL_file("img/data/.user") == 0) {

				std::cout << "Error could not find password database, please reinstall\n";
				throw mx::mxException<std::string>(" Error Password Database not found, please re-install ");
				return;
			}


			std::string user_hash = p.getTags()["user"].tag["id"];
			std::string pass_hash = p.getTags()["user"].tag["pw"];

			if(System::console->hash_pw(old_user->getText()+"xretsam") == user_hash && System::console->hash_pw(old_password->getText()+"xretsam") == pass_hash) {

				// Confirmed change user/password
				p.getTags()["user"].tag["id"] = System::console->hash_pw(new_user->getText()+"xretsam");
				p.getTags()["user"].tag["pw"] = System::console->hash_pw(new_password->getText()+"xretsam");

				std::fstream file;
				file.open("img/data/.user", std::ios::out);
				p.printMXL_byIndex(file);
				file.close();

				cool_box->show();
				setFocus(cool_box->getID());
				setLogin();

			} else {

				msgbox->show();
				msgbox->clr();
				setFocus(msgbox->getID());
				old_user->setText("");
				old_password->setText("");
			}

		}

		void checkPass() {

         	mxlParser p;

			if( p.parseMXL_file("img/data/.user") == 0) {

				std::cout << "Error could not find password database, please reinstall\n";
				msgbox->show();
				msgbox->clr();
				pass->setText("");
				pass->offset = 0;
				user->setText("");
				throw mx::mxException<std::string>("Error could not find password database, please re-install ");
				return;

			}

			std::string user_hash = p.getTags()["user"].tag["id"];
			std::string pass_hash = p.getTags()["user"].tag["pw"];


			if(System::console->hash_pw(user->getText()+"xretsam") == user_hash && System::console->hash_pw(pass->getText()+"xretsam") == pass_hash ) {
				System::console->setLoginName(user->getText());
				mX::server->unlockChange();
				mX::server->setCurDim(1);
				mX::server->removeDimension(0);
				mX::server->setClose(true);
				mX::server->user_id = user->getText();
				user->setText("");
				pass->setText("");
			}
			else {
				msgbox->show();
				msgbox->clr();
				setFocus(msgbox->getID());
				pass->setText("");
				pass->offset = 0;
				pass->cursor.x = 0;
				pass->cursor.y = 0;
				user->setText("");
			}


		}

		virtual void logic() {

			int  rt = msgbox->msgReturn();
			if(rt == 3 )
				setFocus(window->getID());

		}

		int procEvent(widgetAction &we) {

			switch(we.actionCode) {
			case MX_BUTTON_CLICKED: {

				ActionButton<LoginDimension> *bt = (ActionButton<LoginDimension>*)we.extra;

				if(bt->ident() == button->ident()) {

					checkPass();
				}

				if(bt->ident() == change_upw->ident()) {
					changePW();
				}

			}
			break;
			case MX_EDIT_ENTER: {


				ActionLine<LoginDimension> *line = (ActionLine<LoginDimension> *)we.extra;

				if(line->ident() == pass->ident()) {

					checkPass();

				}


			}
			break;
			default:
				return MSG_IGNORED;
			}



			return MSG_IGNORED;
		}

		virtual void  keyDown(int key, void *data) {

			if(key == SDLK_TAB) {

				if(con == 0)  {

					window->clrFocus();
					pass->setFocus(true);
					window->setFocus(pass);
					con = 1;
				}
				else {

					window->clrFocus();
					user->setFocus(true);
					window->setFocus(user);
					con = 0;
				}
			}

			Dimension::keyDown(key,  data);
		}

		void setMode(int m) { login_mode = m; }

	protected:
		int login_id;
		ActionButton<LoginDimension> *button, *change_upw;
		LineEdit *user, *new_user, *old_user, *new_password, *old_password;
		ActionLine<LoginDimension> *pass;
		int user_id, pass_id;
		int con;
		Window *window;
		Window *change;
		MessageBox *msgbox, *cool_box;
		std::string user_ident;
		std::string hashed_pw;
		int login_mode;

	};

}
#endif




