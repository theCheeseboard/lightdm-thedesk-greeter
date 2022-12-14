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
#include "readypane.h"
#include "ui_readypane.h"

#include "sessionmenu.h"

struct ReadyPanePrivate {
    SessionMenu* menu;
};

ReadyPane::ReadyPane(QWidget* parent) :
    QWidget(parent),
    ui(new Ui::ReadyPane) {
    ui->setupUi(this);

    d = new ReadyPanePrivate();
    d->menu = new SessionMenu(this);

    ui->titleLabel->setBackButtonShown(true);
    ui->sessionSelect->setMenu(d->menu);
}

ReadyPane::~ReadyPane() {
    delete d;
    delete ui;
}

void ReadyPane::prompt(QString username, bool isUnlock, QString defaultSession) {
    ui->usernameLabel->setText(tr("Hi, %1!").arg(username));
    ui->sessionSelect->setVisible(!isUnlock);
    ui->messageLabel->setText(isUnlock ? tr("Welcome back!") : tr("Ready to log in?"));
    ui->loginButton->setText(isUnlock ? tr("Unlock") : tr("Log In"));
    ui->loginButton->setFocus();


    QAction* action = d->menu->actionForSession(defaultSession);
    if (!action) action = ui->sessionSelect->menu()->actions().first();
    on_sessionSelect_triggered(action);
}

void ReadyPane::on_titleLabel_backButtonClicked() {
    emit reject();
}

void ReadyPane::on_loginButton_clicked() {
    emit accept();
}

void ReadyPane::on_sessionSelect_triggered(QAction* arg1) {
    ui->sessionSelect->setText(arg1->text());
    emit sessionChanged(arg1->data().toString());
}
