
#include"mxwidget.h"


namespace mxui {

	using mx::Color;
	

	int mxWidget::widget_id = 1;
	

	mxWidget::mxWidget( mxWidget *parent )
	{
		this->parent = parent;
		this_id = widget_id++;
		visible = false;
		type = TYPE_OTHER;
	}
	
	mxWidget::~mxWidget()
	{
		
		
		
	}
	
	void mxWidget::setIdentifier(int x) {
		widget_id =  x;
	}
	
	void mxWidget::createWidget(Rect r)
	{
		setSize(r.rsize);
		setPos(r.point);
		draw_surface.cleanSurface();
		draw_surface.createSurface(r.rsize);
	}
	
	void mxWidget::createWidget(Point spot, Size dim) 
	{
		
		setSize(dim);
		setPos(spot);
		draw_surface.cleanSurface();
		draw_surface.createSurface( dim );
		
	}
		
	void mxWidget::setSize( Size s )
	{
		dimensions.rsize = s;
	}
		
	void mxWidget::setPos( Point p )
	{
		
		dimensions.point = p;
	}
		
	const Point &mxWidget::getPos() const
	{
		return dimensions.point;
		
	}
		
	
	const Size &mxWidget::getSize() const
	{
		
		return dimensions.rsize;
		
	}

	void mxWidget::copyWidgetSurface(mxSurface &surface) 
	{
		SDL_Rect rc = { dimensions.x(), dimensions.y(), dimensions.width(), dimensions.height() };
		surface.copySurface(draw_surface, 0, &rc);
	}
	
	
	void mxWidget::show()
	{
		visible = true;
	}
	
	void mxWidget::hide()
	{
		visible = false;
	}

	int mxWidget::renderEvent()
	{
		mx::mxPainter p(Surface());
		p.fillRect(dimensions, Color(175,150,124));	
		return MSG_PROC;
	}

	void mxWidget::setParent(mxWidget *parent)
	{
		this->parent =  parent;
	}
	
	mxWidget *mxWidget::getParent()
	{
		return this->parent;
	}
			
	void mxWidget::setRect( Rect r)
	{
		dimensions = r;
	}








}

