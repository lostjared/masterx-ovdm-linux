#ifndef _MX_MENU
#define _MX_MENU


#include<mxwidget_type.h>
#include<mxl.h>
#include<mx.h>

namespace mxui {



	using std::vector;
	
	
		
		struct menuInfo {
			
			std::string name;
			std::string id;
			Rect cursor;
		};
		
		typedef  struct Items {
			
			Items() { textcolor = Color(255,0,0); length = 0; }
			
			string FileMenu;
			vector<menuInfo>  menu_item;
			Rect cord;
			Color textcolor;
			int length;
		} MenuItems;

		class Menu : public mxFontDisplay {
			
		public:
			Menu(mxSurface &surface) : render_surface(surface) {
				
				show_menu = -1;
				line = 1;
				callback = 0;
			}
		
			virtual   ~Menu() {}
			void setCallback( void (*call)(int) )  { callback = call; }
			void parseMenu(string menu_name);
			void parseData(mxlParser  &p);
			void draw();
			void active(int  i);
			void  SetLength(int now) { lamer_length =  now; }
			void setIndex(int i) { line = i; }
			void mouseMotion(int x, int y, int button);
			int mouseDown(int x, int y, int button);
			virtual void clicked(int action);
			
			mxSurface  &render_surface;	
			std::vector<MenuItems> menu;
			int show_menu;
			int line;
			void (*callback)(int code);
		protected:
			int lamer_length;
		};	
		
}

#endif
