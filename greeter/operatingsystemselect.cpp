/****************************************
 *
 *   INSERT-PROJECT-NAME-HERE - INSERT-GENERIC-NAME-HERE
 *   Copyright (C) 2021 Victor Tran
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
#include "operatingsystemselect.h"
#include "ui_operatingsystemselect.h"

#include "poweroptions.h"
#include <QDBusInterface>
#include <QDebug>
#include <QDir>
#include <QScreen>
#include <QToolButton>
#include <tpopover.h>
#include <tsettings.h>
#include <tvariantanimation.h>

struct OperatingSystemSelectPrivate {
};

OperatingSystemSelect::OperatingSystemSelect(QList<System> systems, QWidget* parent) :
    QDialog(parent),
    ui(new Ui::OperatingSystemSelect) {
    ui->setupUi(this);
    d = new OperatingSystemSelectPrivate();

    for (System system : systems) {
        QToolButton* button = new QToolButton(this);
        button->setText(system.name);
        button->setIcon(system.icon);
        button->setIconSize(SC_DPI_T(QSize(128, 128), QSize));
        button->setToolButtonStyle(Qt::ToolButtonTextUnderIcon);

        connect(button, &QToolButton::clicked, this, [=] {
            this->triggerSystem(system);
        });

        if (system.isCurrent) {
            ui->systemsLayout->insertWidget(0, button);
        } else {
            ui->systemsLayout->addWidget(button);
        }
    }

    QPalette pal = this->palette();
    pal.setColor(QPalette::Window, Qt::black);
    pal.setColor(QPalette::WindowText, Qt::white);
    this->setPalette(pal);

    ui->titleLabel->setBackButtonIsMenu(true);
    ui->titleLabel->setBackButtonShown(true);
}

void OperatingSystemSelect::triggerSystem(System system) {
    // Start fading out
    QWidget* fadeWidget = new QWidget(this);
    fadeWidget->resize(this->size());
    fadeWidget->move(0, 0);
    fadeWidget->raise();

    QPalette pal = fadeWidget->palette();
    pal.setColor(QPalette::Window, Qt::transparent);
    fadeWidget->setPalette(pal);
    fadeWidget->setAutoFillBackground(true);
    fadeWidget->show();

    tVariantAnimation* anim = new tVariantAnimation(this);
    anim->setStartValue(0);
    anim->setEndValue(255);
    anim->setDuration(250);
    anim->setEasingCurve(QEasingCurve::OutQuint);
    connect(anim, &tVariantAnimation::valueChanged, this, [=](QVariant value) {
        QPalette pal = fadeWidget->palette();
        pal.setColor(QPalette::Window, QColor(0, 0, 0, value.toInt()));
        fadeWidget->setPalette(pal);
    });
    connect(anim, &tVariantAnimation::finished, this, [=] {
        if (system.isCurrent) {
            this->close();
        } else {
            // Boot into the other system
            QDBusInterface logind("org.freedesktop.login1", "/org/freedesktop/login1", "org.freedesktop.login1.Manager", QDBusConnection::systemBus());
            logind.call("SetRebootToBootLoaderEntry", system.id);
            logind.call("Reboot", false);
        }
    });
    anim->start();
}

OperatingSystemSelect::~OperatingSystemSelect() {
    delete d;
    delete ui;
}

void OperatingSystemSelect::select() {
    // Determine whether we should present the selection screen
    tSettings settings;
    if (settings.value("os-chooser/disabled").toBool()) return;

    // Ensure that there are operating systems to select
    QDBusInterface logind("org.freedesktop.login1", "/org/freedesktop/login1", "org.freedesktop.login1.Manager", QDBusConnection::systemBus());
    QDBusMessage canRebootToBootLoaderEntryReply = logind.call("CanRebootToBootLoaderEntry");
    if (canRebootToBootLoaderEntryReply.type() == QDBusMessage::ErrorMessage) return;    // Fail silently
    if (canRebootToBootLoaderEntryReply.arguments().isEmpty()) return;                   // Fail silently
    if (canRebootToBootLoaderEntryReply.arguments().first().toString() != "yes") return; // Bootloader does not support OS selection

    QStringList bootloaderEntries = logind.property("BootLoaderEntries").toStringList();
    bootloaderEntries.removeAll("auto-reboot-to-firmware-setup"); // Don't consider rebooting to firmware setup
    if (bootloaderEntries.isEmpty()) return;

    QString machineId;
    QFile machineIdFile("/etc/machine-id");
    if (!machineIdFile.open(QFile::ReadOnly)) return;
    machineId = machineIdFile.readAll().trimmed();
    machineIdFile.close();

    // Attempt to find the loader directory in the $BOOT partition
    QString loaderDirPath;
    for (QString path : QStringList({"/boot", "/efi", "/boot/efi"})) {
        if (!QDir(path).entryList({"loader"}, QDir::Dirs).isEmpty()) {
            loaderDirPath = path;
            break;
        }
    }
    if (loaderDirPath.isEmpty()) return; // No loader directory

    QDir loaderDir(QDir(loaderDirPath).absoluteFilePath("loader"));
    QDir entriesDir(loaderDir.absoluteFilePath("entries"));

    QList<System> systems;
    bool canDetermineCurrent = false;
    for (QString bootloaderEntry : bootloaderEntries) {
        System system;
        system.id = bootloaderEntry;
        if (bootloaderEntry.startsWith("auto")) {
            if (bootloaderEntry == "auto-windows") {
                system.name = QStringLiteral("Windows");
                system.icon = QIcon(":/systems/windows.svg");
            }
        } else {
            QFile configurationFile(entriesDir.absoluteFilePath(bootloaderEntry));
            qDebug() << configurationFile.fileName();
            if (!configurationFile.open(QFile::ReadOnly)) continue;
            QString line = configurationFile.readLine();
            while (!line.isEmpty()) {
                QStringList parts = line.split(" ");
                QString key = parts.takeFirst().toLower();
                QString value = parts.join(" ").trimmed();

                if (key == "title") {
                    system.name = value;

                    if (value.contains("cactus", Qt::CaseInsensitive)) {
                        system.icon = QIcon(":/systems/cactus.svg");
                    } else {
                        system.icon = QIcon(":/systems/generic.svg");
                    }
                } else if (key == "machine-id") {
                    if (value == machineId) {
                        system.isCurrent = true;
                        canDetermineCurrent = true;
                    }
                }

                line = configurationFile.readLine();
            }
        }

        systems.append(system);
    }

    if (!canDetermineCurrent) return;

    // Don't show the OS selection screen if we are the only OS
    if (systems.count() <= 1) return;

    OperatingSystemSelect* select = new OperatingSystemSelect(systems);
    select->showFullScreen();
    select->setGeometry(QApplication::primaryScreen()->geometry());
    select->exec();
    qDebug() << "Systems found";
}

void OperatingSystemSelect::on_titleLabel_backButtonClicked() {
    PowerOptions* pamChallenge = new PowerOptions();
    tPopover* popover = new tPopover(pamChallenge);
    popover->setPopoverSide(tPopover::Leading);
    popover->setPopoverWidth(SC_DPI(400));
    connect(pamChallenge, &PowerOptions::showCover, this, [=]() {
        popover->dismiss();
    });
    connect(pamChallenge, &PowerOptions::done, popover, &tPopover::dismiss);
    connect(popover, &tPopover::dismissed, pamChallenge, &PowerOptions::deleteLater);
    connect(popover, &tPopover::dismissed, popover, &tPopover::deleteLater);
    popover->show(this);
    pamChallenge->setFocus();
}
