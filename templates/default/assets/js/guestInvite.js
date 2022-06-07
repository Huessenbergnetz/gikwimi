/*
 * SPDX-FileCopyrightText: (C) 2022 Matthias Fehring / www.huessenbergnetz.de
 * SPDX-License-Identifier: AGPL-3.0-or-later
 */

var GikDefTmpl = GikDefTmpl || {};

GikDefTmpl.Guest = GikDefTmpl.Guest || {};

GikDefTmpl.Guest.Invite = GikDefTmpl.Guest.Invite || {};

GikDefTmpl.Guest.Invite.copyToClipboardButton = document.getElementById('copyInvitationToClipboard');

GikDefTmpl.Guest.Invite.textArea = document.getElementById('inviteGuestText');

GikDefTmpl.Guest.Invite.init = function() {
    if (GikDefTmpl.Guest.Invite.copyToClipboardButton) {
        GikDefTmpl.Guest.Invite.copyToClipboardButton.addEventListener('click', function() {
            GikDefTmpl.Guest.Invite.textArea.select();
            GikDefTmpl.Guest.Invite.textArea.setSelectionRange(0, 99999);

            navigator.clipboard.writeText(GikDefTmpl.Guest.Invite.textArea.value);
        })
    }
}

GikDefTmpl.Guest.Invite.init();
