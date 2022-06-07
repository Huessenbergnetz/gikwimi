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
