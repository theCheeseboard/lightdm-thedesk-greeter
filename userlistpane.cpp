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
#include "userlistpane.h"
#include "ui_userlistpane.h"

#include <tpopover.h>
#include "differentuserpopover.h"
#include <QLightDM/UsersModel>
#include <QLightDM/Greeter>

struct UsersListPanePrivate {
    QList<QToolButton*> buttons;
    QLightDM::UsersModel* users;
    QLightDM::Greeter* greeter;
};

UserListPane::UserListPane(QWidget* parent) :
    QWidget(parent),
    ui(new Ui::UserListPane) {
    ui->setupUi(this);

    d = new UsersListPanePrivate();
    d->users = new QLightDM::UsersModel();
    connect(d->users, &QLightDM::UsersModel::dataChanged, this, &UserListPane::updateUsers);
    connect(d->users, &QLightDM::UsersModel::rowsInserted, this, &UserListPane::updateUsers);
    connect(d->users, &QLightDM::UsersModel::rowsRemoved, this, &UserListPane::updateUsers);
    updateUsers();

    if (d->buttons.count() == 1) {
        //Automatically attempt to log in as the first person if there is only one person
        d->buttons.first()->click();
    }

    ui->titleLabel->setBackButtonShown(true);
}

UserListPane::~UserListPane() {
    d->users->deleteLater();
    delete d;
    delete ui;
}

void UserListPane::setGreeter(QLightDM::Greeter* greeter) {
    ui->someoneElseButton->setVisible(greeter->showManualLoginHint());
    ui->guestButton->setVisible(greeter->hasGuestAccountHint());
}

void UserListPane::updateUsers() {
    for (QToolButton* b : d->buttons) {
        ui->usersLayout->removeWidget(b);
        b->deleteLater();
    }
    d->buttons.clear();

    for (int i = 0; i < d->users->rowCount(QModelIndex()); i++) {
        QModelIndex index = d->users->index(i);
        QString username = index.data(QLightDM::UsersModel::NameRole).toString();
        QString displayName = index.data(Qt::DisplayRole).toString();
        QToolButton* b = new QToolButton(this);
        b->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Preferred);
        b->setText(displayName);
        b->setIcon(QIcon::fromTheme("user"));
        b->setToolButtonStyle(Qt::ToolButtonTextBesideIcon);
        connect(b, &QToolButton::clicked, this, [ = ] {
            emit userSelected(displayName, username);
        });
        ui->usersLayout->addWidget(b);
        d->buttons.append(b);
    }
}

void UserListPane::on_titleLabel_backButtonClicked() {
    emit reject();
}

void UserListPane::on_someoneElseButton_clicked() {
    DifferentUserPopover* differentUser = new DifferentUserPopover();
    tPopover* popover = new tPopover(differentUser);
    popover->setPopoverWidth(SC_DPI(400));
    connect(differentUser, &DifferentUserPopover::doLogin, this, [ = ](QString user) {
        emit userSelected(user, user);
    });
    connect(differentUser, &DifferentUserPopover::done, popover, &tPopover::dismiss);
    connect(popover, &tPopover::dismissed, differentUser, &DifferentUserPopover::deleteLater);
    connect(popover, &tPopover::dismissed, popover, &tPopover::deleteLater);
    popover->show(this->window());
    differentUser->setFocus();
}

void UserListPane::on_powerOptionsButton_clicked() {
    showPowerOptions();
}

void UserListPane::on_guestButton_clicked() {
    emit loginGuest();
}
