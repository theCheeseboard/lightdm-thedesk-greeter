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
#include "actionqueue.h"

#include <QQueue>

struct ActionQueuePrivate {
    QQueue<ActionQueue::Action> queue;
    bool actionRunning = false;
};

ActionQueue::ActionQueue(QObject* parent) : QObject(parent) {
    d = new ActionQueuePrivate();
}

ActionQueue::~ActionQueue() {
    delete d;
}

void ActionQueue::enqueue(Action action) {
    d->queue.enqueue(action);
    tryPopNext();
}

void ActionQueue::tryPopNext() {
    if (d->queue.isEmpty() || d->actionRunning) return;

    ActionQueue::Action action = d->queue.dequeue();
    d->actionRunning = true;
    action([ = ] {
        d->actionRunning = false;
        tryPopNext();
    });
}
