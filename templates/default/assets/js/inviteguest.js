/*
 * SPDX-FileCopyrightText: (C) 2022 Matthias Fehring / www.huessenbergnetz.de
 * SPDX-License-Identifier: AGPL-3.0-or-later
 */

var GikDefTmpl = GikDefTmpl || {};

GikDefTmpl.inviteGuest = GikDefTmpl.inviteGuest || {};

GikDefTmpl.inviteGuest.copyToClipboardButton = document.getElementById('copyInvitationToClipboard');

GikDefTmpl.inviteGuest.inviteGuestTextArea = document.getElementById('inviteGuestText');

GikDefTmpl.inviteGuest.init = function() {
    if (GikDefTmpl.inviteGuest.copyToClipboardButton) {
        GikDefTmpl.inviteGuest.copyToClipboardButton.addEventListener('click', function() {
            GikDefTmpl.inviteGuest.inviteGuestTextArea.select();
            GikDefTmpl.inviteGuest.inviteGuestTextArea.setSelectionRange(0, 99999);

            navigator.clipboard.writeText(GikDefTmpl.inviteGuest.inviteGuestTextArea.value);
        })
    }
}

GikDefTmpl.inviteGuest.init();
