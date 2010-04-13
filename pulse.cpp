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

PulseEffect::PulseEffect()
    {
    reconfigure( ReconfigureAll );
    }

void PulseEffect::reconfigure( ReconfigureFlags )
    {
    KConfigGroup conf = EffectsHandler::effectConfig("Pulse");
    mZoomDuration = conf.readEntry("ZoomDuration", 250);
    mPulseDuration = conf.readEntry("PulseDuration", 300);
    mPulseSizeRatio = conf.readEntry("PulseSizeRatio", 1.3);
    QString disableForWindowClass = conf.readEntry("DisableForWindowClass", "yakuake;plasma-desktop");
    foreach ( QString i, disableForWindowClass.split(";") )
        mDisableForWindowClass.insert(i);
    }

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
        mTimeLineWindows[ w ].second.addTime( time );
        if( mTimeLineWindows[ w ].second.value() < 1 )
            data.setTransformed();
        else if ( !mTimeLineWindows[ w ].first )
            { // zoom complete, now pulse
            mTimeLineWindows[ w ].first = true;
            TimeLine& t = mTimeLineWindows[ w ].second;
            t.setCurveShape( TimeLine::EaseOutCurve );
            t.setDuration( animationTime( mPulseDuration ) );
            t.setProgress( 0.0 );
            data.setTransformed();
            }
        else
            mTimeLineWindows.remove( w );
        }
    effects->prePaintWindow( w, data, time );
    }

void PulseEffect::paintWindow( EffectWindow* w, int mask, QRegion region, WindowPaintData& data )
    {
    if( mTimeLineWindows.contains( w ) )
        {
        if ( !mTimeLineWindows[ w ].first )
            { // 1st part: zoom
            double scale = mTimeLineWindows[ w ].second.value();
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
            double scale = 1.0 + mTimeLineWindows[ w ].second.value() * ( mPulseSizeRatio - 1.0 );
            data.opacity *= 1.0 - mTimeLineWindows[ w ].second.value();
            data.xScale *= scale;
            data.yScale *= scale;
            data.xTranslate += int( w->width() / 2 * ( 1 - scale ) );
            data.yTranslate += int( w->height() / 2 * ( 1 - scale ) );
            effects->paintWindow( w, mask, region, data );
            }
        }
    else // disable effect
        effects->paintWindow( w, mask, region, data );        
    }

bool PulseEffect::isPulseWindow( EffectWindow* w )
    {
    if ( w->isManaged() && ( w->isNormalWindow() || w->isDialog() ) )
        {
        const void* e = w->data( WindowAddedGrabRole ).value<void*>();
        if ( e && e != this )
            return false;
        foreach ( QString i , w->windowClass().split(" ") )
            if ( mDisableForWindowClass.contains( i ) )
                return false;
        return true;
        }
    else
        return false;
    }

void PulseEffect::postPaintWindow( EffectWindow* w )
    {
    if( mTimeLineWindows.contains( w ) )
        w->addRepaintFull(); // trigger next animation repaint
    effects->postPaintWindow( w );
    }

void PulseEffect::windowAdded( EffectWindow* w )
    {
    if( w->isOnCurrentDesktop() && isPulseWindow(w) )
        {
        mTimeLineWindows[ w ].first = false; // zoom
        TimeLine& t = mTimeLineWindows[ w ].second;
        t.setCurveShape( TimeLine::LinearCurve );
        t.setDuration( animationTime( mZoomDuration ) );
        t.setProgress( 0.0 );
        w->addRepaintFull();
        }
    }

void PulseEffect::windowClosed( EffectWindow* w )
    {
    mTimeLineWindows.remove( w );
    }

} // namespace
