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

#include "pulse_config.h"

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

KWIN_EFFECT_CONFIG( pulse, PulseEffectConfig )

PulseEffectConfigForm::PulseEffectConfigForm(QWidget* parent) : QWidget(parent)
{
  setupUi(this);
}

PulseEffectConfig::PulseEffectConfig(QWidget* parent, const QVariantList& args) :
        KCModule(EffectFactory::componentData(), parent, args)
    {
    m_ui = new PulseEffectConfigForm(this);

    QVBoxLayout* layout = new QVBoxLayout(this);

    layout->addWidget(m_ui);

    connect(m_ui->spinZoomDuration, SIGNAL(valueChanged(int)), this, SLOT(changed()));
    connect(m_ui->spinPulseDuration, SIGNAL(valueChanged(int)), this, SLOT(changed()));
    connect(m_ui->spinPulseSizeRatio, SIGNAL(valueChanged(double)), this, SLOT(changed()));
    connect(m_ui->editDisableForWindowClass, SIGNAL(textEdited(const QString &)), this, SLOT(changed()));

    load();
    }

void PulseEffectConfig::load()
    {
    KCModule::load();

    KConfigGroup conf = EffectsHandler::effectConfig("Pulse");

    int zoomDuration = conf.readEntry("ZoomDuration", 250);
    int pulseDuration = conf.readEntry("PulseDuration", 300);
    double pulseSizeRatio = conf.readEntry("PulseSizeRatio", 1.3);
    QString disableForWindowClass = conf.readEntry("DisableForWindowClass", "yakuake;plasma-desktop");
    m_ui->spinZoomDuration->setValue(zoomDuration);
    m_ui->spinPulseDuration->setValue(pulseDuration);
    m_ui->spinPulseSizeRatio->setValue(pulseSizeRatio);
    m_ui->editDisableForWindowClass->setText(disableForWindowClass);

    emit changed(false);
    }

void PulseEffectConfig::save()
    {
    KConfigGroup conf = EffectsHandler::effectConfig("Pulse");

    conf.writeEntry("ZoomDuration", m_ui->spinZoomDuration->value());
    conf.writeEntry("PulseDuration", m_ui->spinPulseDuration->value());
    conf.writeEntry("PulseSizeRatio", m_ui->spinPulseSizeRatio->value());
    conf.writeEntry("DisableForWindowClass", m_ui->editDisableForWindowClass->text());

    conf.sync();

    KCModule::save();
    emit changed(false);
    EffectsHandler::sendReloadMessage( "pulse" );
    }

void PulseEffectConfig::defaults()
    {
    m_ui->spinZoomDuration->setValue(250);
    m_ui->spinPulseDuration->setValue(300);
    m_ui->spinPulseSizeRatio->setValue(1.3);
    m_ui->editDisableForWindowClass->setText("yakuake;plasma-desktop");
    emit changed(true);
    }


} // namespace

#include "pulse_config.moc"
