/*
 * SPDX-FileCopyrightText: (C) 2022 Matthias Fehring / www.huessenbergnetz.de
 * SPDX-License-Identifier: AGPL-3.0-or-later
 */

var GikDefTmpl = GikDefTmpl || {};

GikDefTmpl.newXhrHeaders = function() {
    return new Headers({'X-Requested-With': 'XMLHttpRequest'});
}

GikDefTmpl.showError = function(title, text) {
    console.error(title + ": " + text);
    document.getElementById('messageToastIcon').className = 'bi bi-exclamation-diamond text-danger me-2';
    document.getElementById('messageToastTitle').textContent = title;
    document.getElementById('messageToastBody').textContent = text;
    const toast = bootstrap.Toast.getOrCreateInstance(document.getElementById('messageToast'));
    toast.show();
}

GikDefTmpl.updateRangeWithLabel = function()
{
    const divs = document.getElementsByClassName('range-with-label');
    for (let i = 0; i < divs.length; ++i) {
        const range = divs[i].getElementsByTagName('input')[0];
        const span = divs[i].getElementsByTagName('label')[0].getElementsByTagName('span')[0];
        range.addEventListener('input', (e) => {span.textContent = e.target.value});
    }
}

GikDefTmpl.updateRangeWithLabel();
