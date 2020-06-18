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
#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QDialog>

QT_BEGIN_NAMESPACE
namespace Ui {
    class MainWindow;
}
QT_END_NAMESPACE

namespace QLightDM {
    class Greeter;
}

struct MainWindowPrivate;
class MainWindow : public QDialog {
        Q_OBJECT

    public:
        MainWindow(QLightDM::Greeter* greeter, QWidget* parent = nullptr);
        ~MainWindow();

    private slots:
        void on_passwordPage_accept(const QString& response);

        void on_passwordPage_reject();

        void on_userList_userSelected(const QString& userName, const QString& displayName);

        void on_passwordPage_sessionChanged(const QString& session);

        void on_readyPage_accept();

        void on_readyPage_reject();

        void on_readyPage_sessionChanged(const QString& session);

        void on_userList_reject();

        void on_userList_showPowerOptions();

        void on_userList_loginGuest();

    private:
        Ui::MainWindow* ui;
        MainWindowPrivate* d;

        void resizeEvent(QResizeEvent* event);
        void keyPressEvent(QKeyEvent* event);

        void updatePrimaryScreenGeometry();
        void updatePrimaryScreen();

        void resetGreeter();
        void startSession();

        void showPowerOptions();

        void startAuthentication(QString user);

        // QDialog interface
    public slots:
        void reject();
};
#endif // MAINWINDOW_H
