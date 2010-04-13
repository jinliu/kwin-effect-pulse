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

#include "scaleout_config.h"

#include <kwineffects.h>

#include <klocale.h>
#include <kdebug.h>
#include <kconfiggroup.h>
#include <kaction.h>
#include <KShortcutsEditor>

#include <QWidget>
#include <QVBoxLayout>

namespace KWin
{

KWIN_EFFECT_CONFIG( scaleout, ScaleOutEffectConfig )

ScaleOutEffectConfigForm::ScaleOutEffectConfigForm(QWidget* parent) : QWidget(parent)
    {
    setupUi(this);
    }

ScaleOutEffectConfig::ScaleOutEffectConfig(QWidget* parent, const QVariantList& args) :
    KCModule(EffectFactory::componentData(), parent, args)
    {
    m_ui = new ScaleOutEffectConfigForm(this);

    QVBoxLayout* layout = new QVBoxLayout(this);

    layout->addWidget(m_ui);

    connect(m_ui->spinDuration, SIGNAL(valueChanged(int)), this, SLOT(changed()));
    connect(m_ui->editDisableForWindowClass, SIGNAL(textEdited(const QString &)), this, SLOT(changed()));

    load();
    }

void ScaleOutEffectConfig::load()
    {
    KCModule::load();

    KConfigGroup conf = EffectsHandler::effectConfig("ScaleOut");

    int zoomDuration = conf.readEntry("Duration", 250);
    QString disableForWindowClass = conf.readEntry("DisableForWindowClass", "yakuake;plasma-desktop");
    m_ui->spinDuration->setValue(zoomDuration);
    m_ui->editDisableForWindowClass->setText(disableForWindowClass);

    emit changed(false);
    }

void ScaleOutEffectConfig::save()
    {
    KConfigGroup conf = EffectsHandler::effectConfig("ScaleOut");

    conf.writeEntry("Duration", m_ui->spinDuration->value());
    conf.writeEntry("DisableForWindowClass", m_ui->editDisableForWindowClass->text());

    conf.sync();

    KCModule::save();
    emit changed(false);
    EffectsHandler::sendReloadMessage( "scaleout" );
    }

void ScaleOutEffectConfig::defaults()
    {
    m_ui->spinDuration->setValue(250);
    m_ui->editDisableForWindowClass->setText("yakuake;plasma-desktop");
    emit changed(true);
    }


} // namespace

#include "scaleout_config.moc"
