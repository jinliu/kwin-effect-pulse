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

#include "scaleout.h"

namespace KWin
{

KWIN_EFFECT( scaleout, ScaleOutEffect )

ScaleOutEffect::ScaleOutEffect()
    {
    reconfigure( ReconfigureAll );
    }

void ScaleOutEffect::reconfigure( ReconfigureFlags )
    {
    KConfigGroup conf = EffectsHandler::effectConfig("ScaleOut");
    mDuration = conf.readEntry("Duration", 250);
    QString disableForWindowClass = conf.readEntry("DisableForWindowClass", "yakuake;plasma-desktop");
    foreach ( QString i, disableForWindowClass.split(";") )
        mDisableForWindowClass.insert(i);
    }

void ScaleOutEffect::prePaintScreen( ScreenPrePaintData& data, int time )
    {
    if( !mTimeLineWindows.isEmpty() )
        data.mask |= PAINT_SCREEN_WITH_TRANSFORMED_WINDOWS;
    effects->prePaintScreen( data, time );
    }

void ScaleOutEffect::prePaintWindow( EffectWindow* w, WindowPrePaintData& data, int time )
    {
    if( mTimeLineWindows.contains( w ) )
        {
        mTimeLineWindows[ w ].addTime( time );
        if( mTimeLineWindows[ w ].value() < 1 )
            {
            data.setTransformed();
            w->enablePainting( EffectWindow::PAINT_DISABLED_BY_DELETE );
            }
        else
            {
            mTimeLineWindows.remove( w );
            w->unrefWindow();
            }
        }
    effects->prePaintWindow( w, data, time );
    }

void ScaleOutEffect::paintWindow( EffectWindow* w, int mask, QRegion region, WindowPaintData& data )
    {
    if( mTimeLineWindows.contains( w ) )
        {
        double scale = 1 - mTimeLineWindows[ w ].value();
        data.opacity *= scale;
        data.xScale *= scale;
        data.yScale *= scale;
        data.xTranslate += int( w->width() / 2 * ( 1 - scale ) );
        data.yTranslate += int( w->height() / 2 * ( 1 - scale ) );
        effects->paintWindow( w, mask, region, data );
        }
    else // disable effect
        effects->paintWindow( w, mask, region, data );        
    }

void ScaleOutEffect::postPaintWindow( EffectWindow* w )
    {
    if( mTimeLineWindows.contains( w ) )
        w->addRepaintFull(); // trigger next animation repaint
    effects->postPaintWindow( w );
    }

bool ScaleOutEffect::isScaleOutWindow( EffectWindow* w )
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

void ScaleOutEffect::windowClosed( EffectWindow* w )
    {
    if( w->isOnCurrentDesktop() && isScaleOutWindow( w ) )
        {
        TimeLine& t = mTimeLineWindows[ w ];
        t.setCurveShape( TimeLine::EaseInCurve );
        t.setDuration( animationTime( mDuration ) );
        t.setProgress( 0.0 );
        w->refWindow();
        w->addRepaintFull();
        }
    }

void ScaleOutEffect::windowDeleted( EffectWindow* w )
    {
    mTimeLineWindows.remove( w );
    }

} // namespace
