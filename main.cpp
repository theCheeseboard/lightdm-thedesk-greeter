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
#include "mainwindow.h"

#include <QDebug>
#include <tapplication.h>
#include <QLightDM/Greeter>

int main(int argc, char* argv[]) {
    qputenv("QT_QPA_PLATFORMTHEME", "thedesk-platform");
    qputenv("THELIBS_TSETTINGS_DEFAULT_FILES", "theSuite;theDesk.platform;/home/victor/Documents/Apps/theDesk/platform/platformdefaults.conf");

    tApplication a(argc, argv);
    a.setStyle("contemporary");
    a.setShareDir("/usr/share/lightdm-thedesk-greeter/");
    a.installTranslators();

    QLightDM::Greeter* greeter = new QLightDM::Greeter();
    if (!greeter->connectToDaemonSync()) {
        qDebug() << "Can't connect to LightDM daemon";
        return 1;
    }

    MainWindow w(greeter);
    w.showFullScreen();

    return a.exec();
}
