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
#ifndef OPERATINGSYSTEMSELECT_H
#define OPERATINGSYSTEMSELECT_H

#include <QDialog>
#include <QIcon>

namespace Ui {
    class OperatingSystemSelect;
}

struct OperatingSystemSelectPrivate;
class OperatingSystemSelect : public QDialog {
        Q_OBJECT

    public:
        ~OperatingSystemSelect();

        static void select();

    private slots:
        void on_titleLabel_backButtonClicked();

    private:
        struct System {
            QString id;
            QString name;
            QIcon icon;
            bool isCurrent = false;
        };

        explicit OperatingSystemSelect(QList<System> systems, QWidget* parent = nullptr);

        Ui::OperatingSystemSelect* ui;
        OperatingSystemSelectPrivate* d;

        void triggerSystem(System system);
};

#endif // OPERATINGSYSTEMSELECT_H
