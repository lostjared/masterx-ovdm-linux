#include "mxmsgbox.h"


namespace mxui {

	
	MessageBox::MessageBox(string  skinpath, Point pos, Size size, string title, string data, MSG_Type  type) : Window(),  title_data(title), text_data(data), message_type(type)
	{
	    goner = false;
	    msg_return = 0;
			
		createWindow( Color(255,255,255), title, pos, size, Color(255,255,255), Color(0,0,0), Color(255,255,255));	
		useSkin(skinpath);
		
		string strtext = "Yes";
		if(message_type == MSG_OK) {
			strtext = "Ok";
			ok_yes  = new ActionButton<MessageBox>(this);
			ok_yes->createButton( Rect ((size.width/2)-(100/2), size.height-50, 100, 30), strtext, Color(150, 150, 150), Color(200, 200, 200), Color(0,0,0));		
			ok_yes->setConnector(this, &MessageBox::procEvent);		    
		    
		    addWidget(ok_yes);    
		}
		else if(message_type == MSG_YESNO) {
			
			ok_yes =    new ActionButton<MessageBox>(this);
			int pos = (size.width/2)-(125);
			
			ok_yes->createButton( Rect (pos, size.height-50, 100, 30), "Yes", Color(150, 150, 150), Color(200, 200, 200), Color(0,0,0));		
			ok_yes->setConnector(this, &MessageBox::procEvent);		    
			addWidget(ok_yes);
			none_cancel = new ActionButton<MessageBox>(this);
			none_cancel->createButton( Rect (pos+105, size.height-50, 100, 30), "No", Color(150, 150, 150), Color(200, 200, 200), Color(0,0,0));	
			none_cancel->setConnector(this, &MessageBox::procEvent);
			addWidget(none_cancel);
		}
		
		msg_textcolor = Color(255,255,255);
		
	}
	
	int MessageBox::procEvent(widgetAction &we) {
			
		switch(we.actionCode)
		{
		case MX_BUTTON_CLICKED:
		{
			if(message_type == MSG_OK ) {
				ActionButton<MessageBox> *bt = (ActionButton<MessageBox>*)we.extra;
					
				if(bt->ident() == ok_yes->ident()) {		
					gonerOn();
					hide();
					msg_return = 3;
					return MSG_PROC;
				}
			} else if(message_type == MSG_YESNO) {
				ActionButton<MessageBox> *bt = (ActionButton<MessageBox>*)we.extra;	
				if(bt->ident() == ok_yes->ident()) {
					
					// clicked ye
					msg_return = 1;
					return MSG_PROC;
				}
				
				if(bt->ident() == none_cancel->ident()) {
					
					// clicked no
					hide();
					msg_return =2;
					return MSG_PROC;
					
				}
				
			}
		}
		break;
		default:
			return MSG_IGNORED;
			
		}
		return MSG_IGNORED;
		
	}
	
	
	int MessageBox::renderEvent()
	{
		Window::renderEvent();
		int depth = 0;
		font("verdana")->printTextWidth_64(&depth, Surface(), 25,25, dimensions.rsize.width-25, mx::Color::mapRGB(Surface(), msg_textcolor), text_data.c_str());
		
		return MSG_IGNORED;
		
	}
	void MessageBox::setTextColor( Color col ) {
		std::cout << "yes i did  get a color update change\n";
		msg_textcolor = col;
	}
}
