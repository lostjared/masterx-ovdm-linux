#include "mxdb.h"
#include<dirent.h>
#include<sstream>
#include<unistd.h>
#ifndef _WIN32
#include<pwd.h>
#endif
#include<mxwidget_type.h>









namespace masterx {

	bool MX_ls(vector<fileInfo> &vec, string cpath); 

	void mxListView::createListView(Rect pos, Color color)  {
		
		createWidget(pos);
		bg_color = color;
	}
	
	int mxListView::renderEvent() {
		
		mx::mxPainter painter(draw_surface);
		
		painter.fillRect(dimensions, 0x0);
				
		int  counter = 30;
		int offset = browser->offset;
		int across =  15;
		int largest_gap = 15;
			
		
			
		while( static_cast<size_t>(offset) < file_info->size() && across+100 < dimensions.rsize.width && counter < dimensions.rsize.height ) {
			
			while ( static_cast<size_t>(offset) < file_info->size() && counter < dimensions.rsize.height)
			{
		
				int old_offset = offset;
				
				
				fileInfo f = file_info->operator[](offset++);
				int  depth = 0;
				Color file_color;
				std::string  fname;
				
				if(f.dir == true) {
					fname += "[";
					fname += f.name;
					fname += "]";
					file_color = Color(0,0,255);	
				} else {
					fname = f.name;
					file_color = Color(255,0,0);
				}
				
			
				if(browser->current_file == old_offset) {
					file_color = Color(255,255,255);
				}
				
				
				
				font("verdana")->printTextWidth_64(&depth, Surface(), across,counter, across+100, Color::mapRGB(Surface(), file_color), fname.c_str());
				if(depth > largest_gap) largest_gap = depth + 15;
				counter += 30;							
			}
			counter =  30;
			across += largest_gap + 5 + 30;
		}
		

		browser->offset_stop = offset-1;
		
		
		return MSG_PROC;
		
	}
	
	void  mxListView::setDirectory(std::vector<fileInfo> *v) {
		file_info = v;	
	}
	

	dimBrowser::dimBrowser(mx::mxWnd *wnd, mx::Size s) : mxui::Dimension("Dimension Browser", wnd,s)   {
		
		
		display =  new FileDisplay();
				
		setBackground("img/dimbrowser.png");
		
		display->browser = &browser;
		
		
		const char *dir_path = getHomePath();	
		MX_ls(browser.current_directory, dir_path);
		
			
		assert ( display != 0 );
					
		display->createWindow(Color(254,254,254), "Browser Remote", Point(10, s.height-175), Size(s.width-20, 150), Color(255,0,0), Color(0,0,0), Color(0,0,255));	
		display->useSkin("img/skins/mx/blue");
		display->show();
		addWindow(display);
			
		ActionLine<FileDisplay> *line_edit;
		
		line_edit = new ActionLine<FileDisplay>(display, Point(125, 45), 600);
		display->line_edit = line_edit;
		line_edit->setConnector(display, &FileDisplay::action);
		line_edit->setFocus(true);
		line_edit->setText(dir_path);
		
		
		ActionButton<FileDisplay> *lbutton =  new ActionButton<FileDisplay>(display);
		
		lbutton->setConnector(display, &FileDisplay::button_left_cb);
		lbutton->createButton(Rect(10, 100, 100, 30), "Scroll Left", Color(150,150,150), Color(200,200,200), Color(0,0,0));
				
		
		ActionButton<FileDisplay> *rbutton = new ActionButton<FileDisplay>(display);
		
		rbutton->setConnector(display, &FileDisplay::button_right_cb);
		rbutton->createButton(Rect(110,100, 100, 30), "Scroll Right", Color(150,150,150), Color(200,200,200), Color(0,0,0));
				
		display->button_left = lbutton;
		display->button_right = rbutton;
		
		line_id = display->addWidget(line_edit);
		display->addWidget(lbutton);
		display->addWidget(rbutton);
		
		Label *lab  =    new Label(display, "File Path: ", Point(25,  45));
		lab->setDrawSurface(display->Surface());
		lab->setText( Color(255,255,255), "File Path: ");
		display->addWidget(lab);
		
		
		win = new Window();
		
		win->createWindow(Color(254,254,254), "File Browser", Point(s.width/2-(600/2), 40), Size(680, 480), Color(255,0,0), Color(0,0,0), Color(0,0,255));
		
		win->useSkin("img/skins/mx/blue");
	
		assert ( win !=  0 );

		win->show();
		
		addWindow(win);
	
		mxListView *listv =  new mxListView(win);
		
		assert ( listv != 0 );
		
		int width  = win->size().width-30;
		int height = win->size().height-30;
		
		listv->createListView(Rect(10,20, width, height), Color(0,0,0));
		listv->setBrowser(&browser);
		win->addWidget(listv);
		
		const char *path =  getHomePath();
		
		win->setCaption(std::string("File Browser: ") + path);
		win->setTitleColor( Color(255,255,255) );
		MX_ls(browser.current_directory, path);	
		listv->setDirectory(&browser.current_directory);
		
	}
	
	void  dimBrowser::addItem(mxWidget *p) {
		
		win->addWidget(p);
		
	}
		
	void dimBrowser::setPath(opMode mode, string cpath) {
		
		if(mode == MX_FILE) {
			MX_ls(*directory, cpath);
			current = mode;
			path = cpath;
			return;
		}
		
		if(mode == MX_DIM)
			current = mode;
	}
	
	void dimBrowser::keyDown(int key, void *data) {
		
		if(key == SDLK_UP) {
			
			if(browser.current_file > 0)
			browser.current_file--;
			return;
		}
		
		if(key == SDLK_DOWN) {
			
			if(browser.current_file < browser.offset_stop)
				browser.current_file++;
			return;
		}
		
		if(key == SDLK_SPACE) {
			
			std::string  file;
			
			fileInfo f = browser.current_directory[browser.current_file];
			
			if(f.dir) {
			
				string   fname = display->line_edit->getText();
				if(fname[fname.length()-1] != '/') fname += '/';
				fname += f.name;
				
				DIR *d = opendir(fname.c_str());
				if(d) {

					closedir(d);

					browser.offset = 0;
					browser.current_file = 0;
					MX_ls(browser.current_directory, fname);

				}
			}
			
			
		
		}
		
		
		Dimension::keyDown(key, data);
		
	}

	bool MX_ls(vector<fileInfo> &vec, string cpath) {
		

		if(!vec.empty())
			vec.clear();
		
		DIR *d;
		struct dirent *di;
		
		if( (d = opendir(cpath.c_str())) < 0 ) return false;
		
		while ((di = readdir(d)) != 0)   {
			
			size_t len;
			len = strlen(di->d_name);
			int offset = 0;
					
			if(len > 0 && len < 3 && di->d_name[0] == '.')
				continue;
#ifndef _WIN32			
				if(di->d_type == DT_DIR)  
#else
			    if( 0 ) // for windows
#endif
			    {
					fileInfo i;
					i.name =  di->d_name;
					i.dir = true;
					i.offset = offset++;
					vec.push_back(i);
				} else {
					fileInfo f;
					f.name  = di->d_name;
					f.dir = false;
					f.offset = offset++;
					vec.push_back(f);	
				}
				
				
		}
				
		closedir(d);		
		return true;
	}
	

	int FileDisplay::action(widgetAction &ew)
	{
		
		string file_path = line_edit->getText();
	
		DIR *d = opendir(file_path.c_str());
		
		if(!d) {
			std::cout << "Error directory path: " << file_path << " does not exisit\n";
			return MSG_IGNORED;
		}
		
		closedir(d);
		
		browser->offset = 0;
		browser->current_file = 0;
		MX_ls(browser->current_directory, file_path);
		
		return MSG_PROC;
	}
	

	int FileDisplay::button_left_cb(widgetAction &we) {
		
		if(browser->offset > 0) browser->offset --;
		
		return MSG_PROC;
	}
	
	int FileDisplay::button_right_cb(widgetAction &we) {
		
		if(static_cast<unsigned int>(browser->offset) < browser->current_directory.size())
			browser->offset++;
		
		return MSG_PROC;
	}

}


