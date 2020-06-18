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
#include "passwordpane.h"
#include "ui_passwordpane.h"

#include "sessionmenu.h"
#include <QLightDM/SessionsModel>

struct PasswordPanePrivate {
    SessionMenu* menu;
};

PasswordPane::PasswordPane(QWidget* parent) :
    QWidget(parent),
    ui(new Ui::PasswordPane) {
    ui->setupUi(this);

    d = new PasswordPanePrivate();
    d->menu = new SessionMenu(this);

    ui->titleLabel->setBackButtonShown(true);
    ui->sessionSelect->setMenu(d->menu);
}

PasswordPane::~PasswordPane() {
    delete d;
    delete ui;
}

void PasswordPane::prompt(QString username, bool echo, bool isUnlock, QString defaultSession) {
    ui->usernameLabel->setText(tr("Hi, %1!").arg(username));
    ui->sessionSelect->setVisible(!isUnlock);
    ui->password->setEchoMode(echo ? QLineEdit::Normal : QLineEdit::Password);
    ui->password->setText("");
    ui->password->setFocus();

    QAction* action = d->menu->actionForSession(defaultSession);
    if (!action) action = ui->sessionSelect->menu()->actions().first();
    on_sessionSelect_triggered(action);
}

void PasswordPane::on_unlockButton_clicked() {
    emit accept(ui->password->text());
}

void PasswordPane::on_titleLabel_backButtonClicked() {
    emit reject();
}

void PasswordPane::on_sessionSelect_triggered(QAction* arg1) {
    ui->sessionSelect->setText(arg1->text());
    emit sessionChanged(arg1->data().toString());
}
