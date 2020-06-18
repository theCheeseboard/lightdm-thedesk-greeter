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
#include "differentuserpopover.h"
#include "ui_differentuserpopover.h"

DifferentUserPopover::DifferentUserPopover(QWidget* parent) :
    QWidget(parent),
    ui(new Ui::DifferentUserPopover) {
    ui->setupUi(this);

    ui->titleLabel->setBackButtonShown(true);
    this->setFocusProxy(ui->usernameEdit);
}

DifferentUserPopover::~DifferentUserPopover() {
    delete ui;
}

void DifferentUserPopover::on_titleLabel_backButtonClicked() {
    emit done();
}

void DifferentUserPopover::on_nextButton_clicked() {
    emit doLogin(ui->usernameEdit->text());
    emit done();
}
