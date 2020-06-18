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
#include "ui_mainwindow.h"

#include <QApplication>
#include <QScreen>
#include <tpopover.h>
#include "pamchallengepopover.h"
#include "poweroptions.h"
#include <ttoast.h>
#include <tvariantanimation.h>
#include <tsettings.h>
#include <QGraphicsOpacityEffect>
#include <QLightDM/Greeter>
#include <SystemSlide/systemslide.h>
#include "actionqueue.h"

struct MainWindowPrivate {
    QLightDM::Greeter* greeter;
    QString session = "thedesk";

    QScreen* primaryScreen = nullptr;
    QString displayName;
    QString userName;

    //Flag to check if the password pane was shown
    //If not, we show the welcome page at the end
    bool passwordPaneShown = false;

    //Flag to check if we're logging into a guest account
    bool authAsGuest = false;

    //Flag to check if any interactive elements were required
    //If not, and login fails, we don't want to automatically restart login
    //otherwise we'll deadlock ourselves
    bool authAttemptNoInteractive = true;

    //Flag to check if the prompt should be to unlock or log in
    bool isUnlock = false;

    //Flag to check if we're doing an autologin
    bool isAutologin = false;

    ActionQueue* queue;

    QGraphicsOpacityEffect* mainOpacity;

    QPalette originalPal;

    SystemSlide* slide;
};

MainWindow::MainWindow(QLightDM::Greeter* greeter, QWidget* parent)
    : QDialog(parent)
    , ui(new Ui::MainWindow) {
    ui->setupUi(this);
    d = new MainWindowPrivate();
    d->greeter = greeter;
    d->greeter->setResettable(true);

    d->queue = new ActionQueue(this);

    d->originalPal = this->palette();

    d->mainOpacity = new QGraphicsOpacityEffect();
    d->mainOpacity->setEnabled(false);
    ui->mainWidget->setGraphicsEffect(d->mainOpacity);

    connect(qApp, &QApplication::primaryScreenChanged, this, &MainWindow::updatePrimaryScreen);
    updatePrimaryScreen();

    ui->stackedWidget->setCurrentAnimation(tStackedWidget::SlideHorizontal);
    ui->hostnameLabel->setText(d->greeter->hostname());
    ui->userList->setGreeter(d->greeter);

    connect(d->greeter, &QLightDM::Greeter::idle, this, [ = ] {
        this->hide();
    });
    connect(d->greeter, &QLightDM::Greeter::reset, this, [ = ] {
        ui->mainWidget->move(0, 0);
        this->setPalette(d->originalPal);
        d->mainOpacity->setEnabled(false);
        d->mainOpacity->setOpacity(1);
        this->showFullScreen();
    });
    connect(d->greeter, &QLightDM::Greeter::showPrompt, this, [ = ](QString text, QLightDM::Greeter::PromptType type) {
        d->queue->enqueue([ = ](ActionQueue::Continue cont) {
            d->authAttemptNoInteractive = false;
            if (text == "Password: ") {
                //Use the password pane
                ui->stackedWidget->setCurrentWidget(ui->passwordPage);
                ui->passwordPage->prompt(d->displayName, type == QLightDM::Greeter::PromptTypeQuestion, d->isUnlock, sessionForUser(d->userName));
                d->passwordPaneShown = true;
            } else {
                //Use the PAM Challenge pane
                PamChallengePopover* pamChallenge = new PamChallengePopover();
                pamChallenge->setChallenge(text, type == QLightDM::Greeter::PromptTypeQuestion);
                tPopover* popover = new tPopover(pamChallenge);
                popover->setPopoverWidth(SC_DPI(400));
                connect(pamChallenge, &PamChallengePopover::respond, this, [ = ](QString response) {
                    d->greeter->respond(response);
                    ui->mainWidget->setEnabled(false);

                    popover->dismiss();
                });
                connect(pamChallenge, &PamChallengePopover::reject, this, [ = ] {
                    resetGreeter();
                    popover->dismiss();
                });
                connect(popover, &tPopover::dismissed, pamChallenge, &PamChallengePopover::deleteLater);
                connect(popover, &tPopover::dismissed, popover, &tPopover::deleteLater);
                popover->show(ui->mainWidget);
                pamChallenge->setFocus();
            }
            ui->mainWidget->setEnabled(true);

            //Immediately continue because this is not time based
            cont();
        });
    });
    connect(d->greeter, &QLightDM::Greeter::showMessage, this, [ = ](QString text, QLightDM::Greeter::MessageType type) {
        d->queue->enqueue([ = ](ActionQueue::Continue cont) {
            tToast* toast = new tToast(this);
            toast->setTitle(type == QLightDM::Greeter::MessageTypeInfo ? tr("Information") : tr("Error"));
            toast->setText(text);
            toast->setTimeout(3000);
            connect(toast, &tToast::dismissed, this, [ = ] {
                cont();
                toast->deleteLater();
            });
            toast->show(this);
        });
    });
    connect(d->greeter, &QLightDM::Greeter::authenticationComplete, this, [ = ] {
        if (d->greeter->isAuthenticated()) {
            if (d->passwordPaneShown) {
                //Start the session
                startSession();
            } else {
                //Ask the user for their session
                ui->stackedWidget->setCurrentWidget(ui->readyPage);
                ui->readyPage->prompt(d->displayName, d->isUnlock, sessionForUser(d->userName));
                ui->mainWidget->setEnabled(true);
            }
        } else {
            d->queue->enqueue([ = ](ActionQueue::Continue cont) {
                tToast* toast = new tToast(this);
                toast->setTitle(tr("That didn't work"));
                toast->setText(tr("We couldn't authenticate you."));
                toast->setTimeout(3000);
                connect(toast, &tToast::dismissed, [ = ] {
                    cont();
                    toast->deleteLater();
                });
                toast->show(this);
            });

            if (d->authAttemptNoInteractive) {
                //We'll deadlock ourselves if we automatically restart authentication
                d->queue->enqueue([ = ](ActionQueue::Continue cont) {
                    resetGreeter();
                    cont();
                });
            } else {
                //Restart authentication
                startAuthentication(d->userName);
            }
        }
    });
    connect(d->greeter, &QLightDM::Greeter::autologinTimerExpired, this, [ = ] {
        d->isAutologin = true;
        if (d->slide->isActive()) d->slide->deactivate();
        d->greeter->authenticateAutologin();
    });

    connect(d->slide, &SystemSlide::deactivated, this, [ = ] {
        if (!d->isAutologin && d->greeter->autologinUserHint() != "") d->greeter->cancelAutologin();
    });

    d->slide = new SystemSlide(this);
    d->slide->setAction(tr("Log In"), tr("Log in to your device"));
    d->slide->setActionIcon(QIcon::fromTheme("go-up"));
    d->slide->setBackgroundMode(SystemSlide::DesktopBackground);
    d->slide->setDragResultWidget(ui->mainWidget);
    d->slide->setFocus();
}

MainWindow::~MainWindow() {
    d->mainOpacity->deleteLater();
    delete d;
    delete ui;
}

void MainWindow::updatePrimaryScreenGeometry() {
    this->setGeometry(d->primaryScreen->geometry());
}

void MainWindow::updatePrimaryScreen() {
    if (d->primaryScreen) {
        d->primaryScreen->disconnect(this);
    }

    d->primaryScreen = QApplication::primaryScreen();
    connect(d->primaryScreen, &QScreen::geometryChanged, this, &MainWindow::updatePrimaryScreenGeometry);

    updatePrimaryScreenGeometry();
}

void MainWindow::resetGreeter() {
    d->greeter->cancelAuthentication();
    ui->stackedWidget->setCurrentWidget(ui->userList);
    ui->mainWidget->setEnabled(true);
}

void MainWindow::startSession() {
    if (!d->authAsGuest && !d->isUnlock && !d->isAutologin) {
        //Save session details
        QSettings userSettings(d->greeter->ensureSharedDataDirSync(d->userName));
        userSettings.setValue("Login/session", d->session);
    }

    tVariantAnimation* anim1 = new tVariantAnimation(this);
    anim1->setStartValue(0);
    anim1->setEndValue(SC_DPI(-300));
    anim1->setDuration(250);
    anim1->setEasingCurve(QEasingCurve::InCubic);
    connect(anim1, &tVariantAnimation::valueChanged, this, [ = ](QVariant value) {
        ui->mainWidget->move(0, value.toInt());
    });
    connect(anim1, &tVariantAnimation::finished, this, [ = ] {
        d->greeter->startSessionSync(d->session);
        anim1->deleteLater();
    });
    anim1->start();

    tVariantAnimation* anim2 = new tVariantAnimation(this);
    anim2->setStartValue(this->palette().color(QPalette::Window));
    anim2->setEndValue(QColor(0, 0, 0));
    anim2->setDuration(250);
    anim2->setEasingCurve(QEasingCurve::InCubic);
    connect(anim2, &tVariantAnimation::valueChanged, this, [ = ](QVariant value) {
        QPalette pal = this->palette();
        pal.setColor(QPalette::Window, value.value<QColor>());
        this->setPalette(pal);
        this->update();
    });
    connect(anim2, &tVariantAnimation::finished, anim2, &tVariantAnimation::deleteLater);
    anim2->start();

    d->mainOpacity->setEnabled(true);
    tVariantAnimation* anim3 = new tVariantAnimation(this);
    anim3->setStartValue(1.0);
    anim3->setEndValue(0.0);
    anim3->setDuration(250);
    anim3->setEasingCurve(QEasingCurve::InCubic);
    connect(anim3, &tVariantAnimation::valueChanged, this, [ = ](QVariant value) {
        d->mainOpacity->setOpacity(value.toDouble());
    });
    connect(anim3, &tVariantAnimation::finished, anim3, &tVariantAnimation::deleteLater);
    anim3->start();
}

QString MainWindow::sessionForUser(QString user) {
    QSettings userSettings(d->greeter->ensureSharedDataDirSync(user));
    return userSettings.value("Login/session", "").toString();
}

void MainWindow::showPowerOptions() {
    PowerOptions* pamChallenge = new PowerOptions();
    tPopover* popover = new tPopover(pamChallenge);
    popover->setPopoverWidth(SC_DPI(400));
    connect(pamChallenge, &PowerOptions::showCover, this, [ = ]() {
        d->slide->activate();
        popover->dismiss();
    });
    connect(pamChallenge, &PowerOptions::done, popover, &tPopover::dismiss);
    connect(popover, &tPopover::dismissed, pamChallenge, &PowerOptions::deleteLater);
    connect(popover, &tPopover::dismissed, popover, &tPopover::deleteLater);
    popover->show(ui->mainWidget);
    pamChallenge->setFocus();

}

void MainWindow::startAuthentication(QString user) {
    d->passwordPaneShown = false;
    d->authAsGuest = false;
    d->authAttemptNoInteractive = true;
    d->userName = user;

    d->greeter->authenticate(user);
    if (d->greeter->inAuthentication()) {
        ui->mainWidget->setEnabled(false);
        d->isUnlock = d->greeter->lockHint();
    } else {
        tToast* toast = new tToast(this);
        toast->setTitle(tr("That didn't work"));
        toast->setText(tr("We couldn't start the authentication process for %1.").arg(user));
        connect(toast, &tToast::dismissed, toast, &tToast::deleteLater);
        toast->show(this);
    }
}

void MainWindow::on_passwordPage_accept(const QString& response) {
    d->greeter->respond(response);
    ui->mainWidget->setEnabled(false);
}

void MainWindow::on_passwordPage_reject() {
    resetGreeter();
}

void MainWindow::on_userList_userSelected(const QString& displayName, const QString& userName) {
    //Start authenticating a user
    d->displayName = displayName;

    startAuthentication(userName);
}

void MainWindow::on_passwordPage_sessionChanged(const QString& session) {
    d->session = session;
}

void MainWindow::on_readyPage_accept() {
    this->startSession();
}

void MainWindow::on_readyPage_reject() {
    resetGreeter();
}

void MainWindow::on_readyPage_sessionChanged(const QString& session) {
    d->session = session;
}

void MainWindow::resizeEvent(QResizeEvent* event) {
    ui->mainWidget->setFixedSize(QSize(this->width(), this->height()));
}

void MainWindow::keyPressEvent(QKeyEvent* event) {
    if (d->slide->isActive()) d->slide->deactivate();
}

void MainWindow::on_userList_reject() {
    d->slide->activate();
}

void MainWindow::reject() {
    //Do nothing
}

void MainWindow::on_userList_showPowerOptions() {
    showPowerOptions();
}

void MainWindow::on_userList_loginGuest() {
    d->authAsGuest = true;
    d->greeter->authenticateAsGuest();
}
