/*
 * SPDX-FileCopyrightText: (C) 2022 Matthias Fehring / www.huessenbergnetz.de
 * SPDX-License-Identifier: AGPL-3.0-or-later
 */

var GikDefTmpl = GikDefTmpl || {};

GikDefTmpl.Guest = GikDefTmpl.Guest || {};

GikDefTmpl.Guest.setStatusIcon = function(icon, status) {
    if (status === 1) { // Agreed
        icon.className = 'bi bi-check-circle text-success';
    } else if (status === 2) { // Canceled
        icon.className = 'bi bi-x-circle text-warning';
    } else { // DefaultStatus
        icon.className = 'bi bi-dash-circle text-secondary';
    }
}

GikDefTmpl.Guest.updateCount = function(groupId, adultsAccepted, adultsInvited, childrenAccepted, childrenInvited) {
    GikDefTmpl.sumCount(document.getElementById('total_accepted'), adultsAccepted + childrenAccepted);

    GikDefTmpl.sumCount(document.getElementById('total_invited'), adultsInvited + childrenInvited);

    GikDefTmpl.sumCount(document.getElementById('adults_accepted'), adultsAccepted);

    GikDefTmpl.sumCount(document.getElementById('adults_invited'), adultsInvited);

    GikDefTmpl.sumCount(document.getElementById('children_accepted'), childrenAccepted);

    GikDefTmpl.sumCount(document.getElementById('children_invited'), childrenInvited);

    const section = document.getElementById('group-' + groupId);

    GikDefTmpl.sumCount(section.getElementsByClassName('total_accepted')[0], adultsAccepted + childrenAccepted);

    GikDefTmpl.sumCount(section.getElementsByClassName('total_invited')[0], adultsInvited + childrenInvited);

    GikDefTmpl.sumCount(section.getElementsByClassName('adults_accepted')[0], adultsAccepted);

    GikDefTmpl.sumCount(section.getElementsByClassName('adults_invited')[0], adultsInvited);

    GikDefTmpl.sumCount(section.getElementsByClassName('children_accepted')[0], childrenAccepted);

    GikDefTmpl.sumCount(section.getElementsByClassName('children_invited')[0], childrenInvited);
}
