/****************************************
 *
 *   INSERT-PROJECT-NAME-HERE - INSERT-GENERIC-NAME-HERE
 *   Copyright (C) 2020 Victor Tran
 *
 *   This program is free software: you can redistribute it and/or modify
 *   it under the terms of the GNU General Public License as published by
 *   the Free Software Foundation, either version 3 of the License, or
 *   (at your option) any later version.
 *
 *   This program is distributed in the hope that it will be useful,
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *   GNU General Public License for more details.
 *
 *   You should have received a copy of the GNU General Public License
 *   along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 * *************************************/
#include "poweroptions.h"
#include "ui_poweroptions.h"

#include <Wm/desktopwm.h>
#include <QLightDM/Power>

struct PowerOptionsPrivate {
    QLightDM::PowerInterface* power;
};

PowerOptions::PowerOptions(QWidget* parent) :
    QWidget(parent),
    ui(new Ui::PowerOptions) {
    ui->setupUi(this);

    d = new PowerOptionsPrivate();
    d->power = new QLightDM::PowerInterface();

    ui->powerOffButton->setVisible(d->power->canShutdown());
    ui->rebootButton->setVisible(d->power->canRestart());
    ui->suspendButton->setVisible(d->power->canSuspend());
    ui->hibernateButton->setVisible(d->power->canHibernate());

    ui->titleLabel->setBackButtonShown(true);
}

PowerOptions::~PowerOptions() {
    d->power->deleteLater();
    delete d;
    delete ui;
}

void PowerOptions::on_titleLabel_backButtonClicked() {
    emit done();
}

void PowerOptions::on_powerOffButton_clicked() {
    d->power->shutdown();
}

void PowerOptions::on_rebootButton_clicked() {
    d->power->restart();
}

void PowerOptions::on_suspendButton_clicked() {
    emit showCover();
    d->power->suspend();
}

void PowerOptions::on_turnOffScreenButton_clicked() {
    emit showCover();
    DesktopWm::setScreenOff(true);
}

void PowerOptions::on_hibernateButton_clicked() {
    emit showCover();
    d->power->hibernate();
}
