
#ifndef _ORANGES_
#define _ORANGES_

#include"menu.h"
#include<mxinterface.h>
#include<mx_font.h>

namespace mxui  {


// implementations


		void Menu::parseMenu(string   menu_name) {
			mxlParser p;
			if(!p.parseMXL_file(menu_name)) throw mx::mxException<std::string>("mxl: Error parsing menu file: " + menu_name + "\n");
			parseData(p);
		}

		void Menu::parseData(mxlParser &p) {


			if(p.size() == 0) return;

			mxlArray varray;
			p.fetchArray(varray);

			for(unsigned int i = 0; i < varray.size(); i++) {

				string key = varray[i].first;

				MenuItems menuz;

				menuz.FileMenu =  key;

				for(subtag tag = p.begin_mxl(i, varray); tag != p.end_mxl(i, varray); tag++)  {

					menuInfo info;

					info.name  = tag->first;
					info.id = tag->second;

					menuz.menu_item.push_back( info  );

				}

				menu.push_back(menuz);
			}

		}


		void Menu::draw() {
			size_t i = 0;

			int   start_offset  =  8;

			mx::mxPainter paint(render_surface);

			Rect rectx(0, 0,  lamer_length, 25);
			paint.fillRect(rectx, Color(176,176,176));
			Rect rcx =  rectx;

			rcx.point.x += 4;
			rcx.point.y += 4;
			rcx.rsize.width -= 8;
			rcx.rsize.height -=  8;

			paint.fillRect(rcx, Color(200,200,200));



			for(i = 0; i < menu.size(); i++) {

				menu[i].cord.point = mx::Point(start_offset, 3);
				int pos = font("verdana")->printTextDepth(render_surface, start_offset, 3, mx::Color::mapRGB(render_surface,menu[i].textcolor),  menu[i].FileMenu);

				start_offset += pos+15;
				menu[i].cord.rsize = Size(pos+15, 20);
			}

			if(show_menu != -1 && show_menu < menu.size())
			{
				int calculateHeight =  25;

				for(size_t i = 0; i < menu[show_menu].menu_item.size()-1; i++ )
				{
					calculateHeight +=  25;
				}

				Rect rc( menu[show_menu].cord.point.x , 23, 200, calculateHeight  );
				mx::mxPainter  p(render_surface);
				p.fillRect(rc, Color(150,150,150));
				rc.point.x +=  4;
				rc.point.y += 4;
				rc.rsize.width -= 8;
				rc.rsize.height -= 8;
				p.fillRect(rc, Color(200,200,200));
				p.lock();
				p.drawVertical(0,lamer_length,  0, Color(100,100,100));
				p.unlock();

				int down = 24;



				for(size_t i = 0; i < menu[show_menu].menu_item.size(); i++) {
					int depth = 0;
					int offset_X =  menu[show_menu].cord.point.x+10;
					int offset_X2 = 200;

					Rect rc(offset_X,down+5,offset_X2,25);
					menu[show_menu].menu_item[i].cursor = rc;

					unsigned int color = mx::Color::mapRGB(render_surface,0,0,255);

					font("verdana")->printTextWidth_64(&depth, render_surface, menu[show_menu].cord.point.x+10, down+5, menu[show_menu].cord.point.x+200, color, menu[show_menu].menu_item[i].name.c_str());
					if(line != -1 && i == static_cast<unsigned int>(line)) {

						mx::mxPainter  p(render_surface);
						p.lock();
						p.drawVertical(rc.point.x, rc.point.x+depth, down+20, Color(0,0,255));
						p.unlock();

					}

					down += 24;
				}

			}

		}

		void Menu::active(int  i)
		{
			show_menu = i;
		}

		void Menu::mouseMotion(int x, int y, int button)
		{

			size_t i;
			for(i = 0; i < menu.size(); i++) {

				if(menu[i].cord.pointInRect(Point(x,y))) {
					menu[i].textcolor = Color(0,0,255);
				} else {
					menu[i].textcolor = Color(255,0,0);
				}
			}

			if(show_menu == -1) return;



			for(i = 0; i < menu[show_menu].menu_item.size(); i++) {

				Rect rc = menu[show_menu].menu_item[i].cursor;

				if(rc.pointInRect(Point(x,y)))   {

					setIndex(i);
					return;
				}

			}

			setIndex(-1);

		}


		int Menu::mouseDown(int x, int y, int button)
		{

			if(x > 0 && x < lamer_length && y > 0 && y <= 25)
			{

			size_t i;
			for(i = 0; i < menu.size(); i++) {
				if(menu[i].cord.pointInRect(Point(x,y))) {
					active(i);
					setIndex(-1);
					return -1;
				} else   menu[i].textcolor = Color(255,0,0);

			}

				show_menu = -1;
				return -1;
			}


			size_t i =   0;

			if(show_menu == -1) return -1;

			for(i = 0; i < menu[show_menu].menu_item.size(); i++) {

				Rect rc = menu[show_menu].menu_item[i].cursor;

				if(rc.pointInRect(Point(x,y)))   {

					int asc = atoi(menu[show_menu].menu_item[i].id.c_str());
					clicked(asc);


					return asc;
				}
			}


			show_menu = -1;
			setIndex(-1);
			return -1;
		}


		void Menu::clicked(int action_id) {

			show_menu = -1;
			if(callback != 0)
			callback(action_id);

		}


}

#endif

