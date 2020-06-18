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
#include "sessionmenu.h"

#include <QLightDM/SessionsModel>

struct SessionMenuPrivate {
    QLightDM::SessionsModel* model;
};

SessionMenu::SessionMenu(QWidget* parent) : QMenu(parent) {
    d = new SessionMenuPrivate();
    d->model = new QLightDM::SessionsModel();
    connect(d->model, &QLightDM::SessionsModel::dataChanged, this, &SessionMenu::updateSessions);
    connect(d->model, &QLightDM::SessionsModel::rowsInserted, this, &SessionMenu::updateSessions);
    connect(d->model, &QLightDM::SessionsModel::rowsRemoved, this, &SessionMenu::updateSessions);
    updateSessions();
}

SessionMenu::~SessionMenu() {
    d->model->deleteLater();
    delete d;
}

QAction* SessionMenu::actionForSession(QString session) {
    for (QAction* action : this->actions()) {
        if (action->data().toString() == session) {
            return action;
        }
    }
    return nullptr;
}

void SessionMenu::updateSessions() {
    while (!this->isEmpty()) {
        QAction* action = this->actions().first();
        this->removeAction(action);
        action->deleteLater();
    }

    for (int i = 0; i < d->model->rowCount(QModelIndex()); i++) {
        QModelIndex index = d->model->index(i);

        QAction* a = new QAction(this);
        a->setText(index.data(Qt::DisplayRole).toString());
        a->setData(index.data(QLightDM::SessionsModel::KeyRole).toString());
        this->addAction(a);
    }
}
