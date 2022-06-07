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

GikDefTmpl.updateRangeWithLabel = function() {
    const divs = document.getElementsByClassName('range-with-label');
    for (let i = 0; i < divs.length; ++i) {
        const range = divs[i].getElementsByTagName('input')[0];
        const span = divs[i].getElementsByTagName('label')[0].getElementsByTagName('span')[0];
        range.addEventListener('input', (e) => {span.textContent = e.target.value});
    }
}

GikDefTmpl.composeAddress = function(address) {
    let addressHtml = '';

    if (address.street) {
        addressHtml += address.street + '<br>';
    }
    if (address.postalCode) {
        addressHtml += address.postalCode + ' ';
    }
    if (address.locality) {
        addressHtml += address.locality;
    }
    if (address.locality || address.postalCode) {
        addressHtml += '<br>';
    }
    addressHtml += address.region;
    if (address.region && address.country) {
        addressHtml += ', ' + address.country;
    }

    return addressHtml;
}

GikDefTmpl.switchButton = function(button) {
    if (button.disabled) {
        button.disabled = false;
        const spinner = button.getElementsByTagName('span')[0];
        spinner.classList.remove('d-inline-block');
        spinner.classList.add('d-none');
        const icon = button.getElementsByTagName('i')[0];
        icon.classList.remove('d-none');
    } else {
        button.disabled = true;
        const spinner = button.getElementsByTagName('span')[0];
        spinner.classList.remove('d-none');
        spinner.classList.add('d-inline-block');
        const icon = button.getElementsByTagName('i')[0];
        icon.classList.add('d-none');
    }
}

GikDefTmpl.setFormFieldErrors = function(form, errors) {
    for (const field in errors) {
        if (errors.hasOwnProperty(field)) {
            const formField = form.elements.namedItem(field);
            const feedback = formField.nextElementSibling;
            feedback.innerHTML = errors[field].join('<br>');
            formField.classList.add('is-invalid');
        }
    }
}

GikDefTmpl.resetFormFieldErrors = function(form) {
    for (let i = 0; i < form.elements.length; ++i) {
        form.elements[i].classList.remove('is-invalid');
    }
}

GikDefTmpl.updateRangeWithLabel();
