/*
 * SPDX-FileCopyrightText: (C) 2022 Matthias Fehring / www.huessenbergnetz.de
 * SPDX-License-Identifier: AGPL-3.0-or-later
 */

var GikDefTmpl = GikDefTmpl || {};

GikDefTmpl.editGuest = GikDefTmpl.editGuest || {};

GikDefTmpl.editGuest.modal = null;

GikDefTmpl.editGuest.form = null;

GikDefTmpl.editGuest.button = null;

GikDefTmpl.editGuest.buttons = null;

GikDefTmpl.editGuest.loadData = function(event) {
    const button = event.target;
    button.setAttribute('disabled', 'disabled');
    const span = button.getElementsByTagName('span')[0];
    span.classList.remove('d-none');
    span.classList.add('d-inline-block');
    const icon = button.getElementsByTagName('i')[0];
    icon.classList.add('d-none');
    const guestId = button.value;
    const hdrs = GikDefTmpl.newXhrHeaders();
    const actionPath = GikDefTmpl.editGuest.form.attributes.getNamedItem('action').value + guestId;
    document.getElementById('editGuestId').value = guestId;

    fetch(actionPath, {
              method: 'GET',
              headers: hdrs
          })
    .then(response => {
              if (response.ok) {
                  return response.json();
              } else {
                  return Promise.reject(response);
              }
          })
    .then(data => {
              document.getElementById('editGuestName').textContent = data.contact.addressee.realName;
              document.getElementById('editGuestStatus_' + data.status).checked = true;

              const els = GikDefTmpl.editGuest.form.elements;

              els.namedItem('type').value = data.type;
              els.namedItem('salutation').value = data.salutation;
              els.namedItem('partnerGivenName').value = data.partnerGivenName;
              els.namedItem('partnerFamilyName').value = data.partnerFamilyName;

              const e = new Event('input');

              const ea = els.namedItem('expectedAdults');
              ea.value = data.adults;
              ea.dispatchEvent(e);

              const aa = els.namedItem('agreedAdults');
              aa.value = data.adultsAccepted;
              aa.dispatchEvent(e);

              const ec = els.namedItem('expectedChildren');
              ec.value = data.children;
              ec.dispatchEvent(e);

              const ac = els.namedItem('agreedChildren');
              ac.value = data.childrenAccepted;
              ac.dispatchEvent(e);

              GikDefTmpl.editGuest.modal.show();
          })
    .catch(error => {
               if (error instanceof Response) {
                   error.json().then(json => {
                                         GikDefTmpl.showError(json.error_title, json.error_text);
                                     })
               } else {
                   GikDefTmpl.showError(error.name, error.message);
               }
           })
    .finally(() => {
                 span.classList.remove('d-inline-block');
                 span.classList.add('d-none');
                 icon.classList.remove('d-none');
                 button.removeAttribute('disabled');
            });
}

GikDefTmpl.editGuest.exec = function() {
    const formData = new FormData(GikDefTmpl.editGuest.form);
    const guestId = document.getElementById('editGuestId').value;
    const actionPath = GikDefTmpl.editGuest.form.action + guestId;
    const hdrs = GikDefTmpl.newXhrHeaders();

    fetch(actionPath, {
              method: 'POST',
              headers: hdrs,
              body: formData
          })
    .then(response => response.json())
    .then(guest => {
            console.log(guest);
          })
    .catch(error => {
               if (error instanceof Response) {
                   error.json().then(json => {
                                         GikDefTmpl.showError(json.error_title, json.error_text);
                                     })
               } else {
                   GikDefTmpl.showError(error.name, error.message);
               }
           })
    .finally(() => {
             });
}

GikDefTmpl.editGuest.init = function() {
    const egm = document.getElementById('editGuestModal');
    if (egm) {
        GikDefTmpl.editGuest.modal = bootstrap.Modal.getOrCreateInstance(egm);

        GikDefTmpl.editGuest.buttons = document.getElementsByClassName('edit-guest-btn');
        for (let i = 0; i < GikDefTmpl.editGuest.buttons.length; ++i) {
            const btn = GikDefTmpl.editGuest.buttons.item(i);
            btn.addEventListener('click', GikDefTmpl.editGuest.loadData);
        }

        GikDefTmpl.editGuest.form = document.forms['editGuestForm'];
        GikDefTmpl.editGuest.form.addEventListener('submit', (e) => { e.preventDefault(); GikDefTmpl.editGuest.exec(); });

        GikDefTmpl.editGuest.button = document.getElementById('editGuestButton');
        GikDefTmpl.editGuest.button.addEventListener('click', GikDefTmpl.editGuest.exec);
    }
}

GikDefTmpl.editGuest.init();
