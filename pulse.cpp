/********************************************************************
Copyright (C) 2010 Jin Liu <m.liu.jin@gmail.com>

This program is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation; either version 2 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program.  If not, see <http://www.gnu.org/licenses/>.
*********************************************************************/

#include "pulse.h"

namespace KWin
{

KWIN_EFFECT( pulse, PulseEffect )

void PulseEffect::prePaintScreen( ScreenPrePaintData& data, int time )
    {
    if( !mTimeLineWindows.isEmpty() )
        data.mask |= PAINT_SCREEN_WITH_TRANSFORMED_WINDOWS;
    effects->prePaintScreen( data, time );
    }

void PulseEffect::prePaintWindow( EffectWindow* w, WindowPrePaintData& data, int time )
    {
    if( mTimeLineWindows.contains( w ) )
        {
        mTimeLineWindows[ w ].addTime( time );
        if( mTimeLineWindows[ w ].value() < 1 )
            data.setTransformed();
        else
            mTimeLineWindows.remove( w );
        }
    effects->prePaintWindow( w, data, time );
    }

void PulseEffect::paintWindow( EffectWindow* w, int mask, QRegion region, WindowPaintData& data )
    {
    if( mTimeLineWindows.contains( w ) && isPulseWindow( w ) )
        {
            if ( mTimeLineWindows[ w ].value() < 0.5 )
            { // 1st part: zoom
                double scale = mTimeLineWindows[ w ].value() * 2;
                data.opacity *= scale;
                data.xScale *= scale;
                data.yScale *= scale;
                data.xTranslate += int( w->width() / 2 * ( 1 - scale ) );
                data.yTranslate += int( w->height() / 2 * ( 1 - scale ) );
                effects->paintWindow( w, mask, region, data );
            }
            else
            { // 2nd part: pulse
                effects->paintWindow( w, mask, region, data );
                double scale = 1.0 + ( mTimeLineWindows[ w ].value() - 0.5 ) * 2 * 0.3;
                data.opacity *= 1.0 - ( mTimeLineWindows[ w ].value() - 0.5 ) * 2;
                data.xScale *= scale;
                data.yScale *= scale;
                data.xTranslate += int( w->width() / 2 * ( 1 - scale ) );
                data.yTranslate += int( w->height() / 2 * ( 1 - scale ) );
                effects->paintWindow( w, mask, region, data );
            }
        }
    else
        effects->paintWindow( w, mask, region, data );        
    }

bool PulseEffect::isPulseWindow( EffectWindow* w )
    {
    const void* e = w->data( WindowAddedGrabRole ).value<void*>();
    if ( w->isPopupMenu() || w->isSpecialWindow() || w->isUtility() || ( e && e != this ))
        return false;
    return true;
    }

void PulseEffect::postPaintWindow( EffectWindow* w )
    {
    if( mTimeLineWindows.contains( w ) )
        w->addRepaintFull(); // trigger next animation repaint
    effects->postPaintWindow( w );
    }

void PulseEffect::windowAdded( EffectWindow* c )
    {
    if( c->isOnCurrentDesktop())
        {
        mTimeLineWindows[ c ].setCurveShape( TimeLine::EaseOutCurve );
        mTimeLineWindows[ c ].setDuration( animationTime( 500 ));
        mTimeLineWindows[ c ].setProgress( 0.0 );
        c->addRepaintFull();
        }
    }

void PulseEffect::windowClosed( EffectWindow* c )
    {
    mTimeLineWindows.remove( c );
    }

} // namespace
