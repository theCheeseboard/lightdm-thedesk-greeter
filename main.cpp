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
#include <QFile>
#include <QProcess>
#include <tapplication.h>
#include <QLightDM/Greeter>
#include <QMessageBox>
#include <tsettings.h>
#include "operatingsystemselect.h"

int main(int argc, char* argv[]) {
    qputenv("QT_QPA_PLATFORMTHEME", "thedesk-platform");
    qputenv("THELIBS_TSETTINGS_DEFAULT_FILES", "theSuite;theDesk.platform;/home/victor/Documents/Apps/theDesk/platform/platformdefaults.conf");

    tApplication a(argc, argv);
    a.setApplicationName("lightdm-thedesk-greeter");
    a.setOrganizationName("theSuite");
    a.setStyle("contemporary");
    a.setShareDir("/usr/share/lightdm-thedesk-greeter/");
    a.installTranslators();

    tSettings::registerDefaults("/etc/theSuite/lightdm-thedesk-greeter/defaults.conf");

    if (!QFile::exists("/tmp/lightdm-thedesk-greeter-first-boot")) {
        //Connect to logind and ask to choose an OS
        OperatingSystemSelect::select();

        QFile file("/tmp/lightdm-thedesk-greeter-first-boot");
        file.open(QFile::WriteOnly);
        file.close();
    }

    QLightDM::Greeter* greeter = new QLightDM::Greeter();
    if (!greeter->connectToDaemonSync()) {
        qDebug() << "Can't connect to LightDM daemon";
        return 1;
    }

    if (QFile::exists("/usr/bin/scallop-onboarding-service")) {
        QProcess onboardingServiceProcess;
        onboardingServiceProcess.start("/usr/bin/scallop-onboarding-service", QStringList());
        onboardingServiceProcess.waitForFinished(-1);
    }

    greeter->setResettable(true);
    a.setQuitOnLastWindowClosed(false);

    MainWindow* mainWin = new MainWindow(greeter);
    mainWin->showFullScreen();

    QObject::connect(greeter, &QLightDM::Greeter::reset, [ =, &mainWin] {
        mainWin->hide();
        mainWin->deleteLater();

        MainWindow* mainWin = new MainWindow(greeter);
        mainWin->showFullScreen();
    });

    return a.exec();
}
