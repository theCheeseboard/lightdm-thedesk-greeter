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
#ifndef USERLISTPANE_H
#define USERLISTPANE_H

#include <QWidget>

namespace Ui {
    class UserListPane;
}

namespace QLightDM {
    class Greeter;
}

struct UsersListPanePrivate;
class UserListPane : public QWidget {
        Q_OBJECT

    public:
        explicit UserListPane(QWidget* parent = nullptr);
        ~UserListPane();

        void setGreeter(QLightDM::Greeter* greeter);

    signals:
        void showPowerOptions();
        void userSelected(QString displayName, QString username);
        void reject();
        void loginGuest();

    private slots:
        void on_titleLabel_backButtonClicked();

        void on_someoneElseButton_clicked();

        void on_powerOptionsButton_clicked();

        void on_guestButton_clicked();

    private:
        Ui::UserListPane* ui;
        UsersListPanePrivate* d;

        void updateUsers();
};

#endif // USERLISTPANE_H
