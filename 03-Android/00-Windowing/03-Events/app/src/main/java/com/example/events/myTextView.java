package com.example.events;

import androidx.appcompat.widget.AppCompatTextView;
import android.content.Context;
import android.graphics.Color;

import android.view.Gravity;
import android.view.GestureDetector;
import android.view.GestureDetector.OnDoubleTapListener;
import android.view.GestureDetector.OnGestureListener;
import android.view.MotionEvent;

public class myTextView extends AppCompatTextView implements OnDoubleTapListener, OnGestureListener
{
	private GestureDetector gestureDetector;
	
	myTextView(Context context)
	{
		super(context);

		setText("Hello World!");
		setTextColor(Color.rgb(0,255,0));
		setTextSize(32);
		setBackgroundColor(Color.rgb(0,0,0));
		setGravity(Gravity.CENTER);

		gestureDetector = new GestureDetector(context, this, null, false);
		gestureDetector.setOnDoubleTapListener(this);

	}

	@Override
	public boolean onTouchEvent(MotionEvent event)
	{
		int eventaction = event.getAction();
		if(!gestureDetector.onTouchEvent(event))
			super.onTouchEvent(event);

		return(true);
	}


	@Override
	public boolean onDoubleTap(MotionEvent e)
	{
		setText("Double Tap!");
		return(true);
	}


	@Override
	public boolean onDoubleTapEvent(MotionEvent e)
	{
		return(true);
	}


	@Override
	public boolean onSingleTapConfirmed(MotionEvent e)
	{
		setText("Single Tap!");
		return(true);
	}


	@Override
	public boolean onDown(MotionEvent e)
	{
		return(true);
	}


	@Override
	public boolean onFling(MotionEvent e1, MotionEvent e2, float velocityX, float velocityY)
	{
		return(true);
	}


	@Override
	public void onLongPress(MotionEvent e)
	{
		setText("Long Press!");
	}


	@Override
	public void onShowPress(MotionEvent e)
	{
		//code
	}


	@Override
	public boolean onScroll(MotionEvent e1, MotionEvent e2, float distanceX, float distanceY)
	{
		setText("Scroll!");
		System.exit(0);
		return(true);
	}



	@Override
	public boolean onSingleTapUp(MotionEvent e)
	{
		return(true);
	}

}

