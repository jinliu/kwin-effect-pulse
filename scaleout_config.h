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

#ifndef KWIN_SCALEOUT_CONFIG_H
#define KWIN_SCALEOUT_CONFIG_H

#include <kcmodule.h>

#include "ui_scaleout_config.h"

namespace KWin
{

class ScaleOutEffectConfigForm : public QWidget, public Ui::ScaleOutEffectConfigForm
    {
    Q_OBJECT
    public:
        explicit ScaleOutEffectConfigForm(QWidget* parent);
    };

class ScaleOutEffectConfig : public KCModule
    {
    Q_OBJECT
    public:
        explicit ScaleOutEffectConfig(QWidget* parent = 0, const QVariantList& args = QVariantList());

        virtual void save();
        virtual void load();
        virtual void defaults();

    private:
        ScaleOutEffectConfigForm* m_ui;
    };

} // namespace

#endif
