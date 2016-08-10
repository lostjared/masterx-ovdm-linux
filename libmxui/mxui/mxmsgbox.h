#ifndef   _MSG_BOX_X_
#define   _MSG_BOX_X_

#include "mxwindow.h"


namespace mxui {


enum MSG_Type { MSG_OK, MSG_YESNO };

class MessageBox : public Window {
	
public:
	MessageBox(string skinpath, Point pos, Size size, string title,string text, MSG_Type type);
	int procEvent(widgetAction &we);
	virtual int renderEvent();
	const bool gone() const { return goner; }
	void clr() { goner = false; msg_return = 0;; }
	void setTextColor( Color col );
	const int  msgReturn() const { return msg_return; }
	
private:
	MessageBox(const MessageBox &);
	MessageBox &operator=(const MessageBox &);	
protected:
	string  title_data;
	string  text_data;
	MSG_Type message_type;
	ActionButton<MessageBox>  *ok_yes, *none_cancel;
	bool goner;
	int msg_return;
private:
	Color msg_textcolor;
	
};











}


#endif



