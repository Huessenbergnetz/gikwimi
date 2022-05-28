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
              document.getElementById('editGuestType').value = data.type;
              document.getElementById('editGuestSalutation').value = data.salutation;
              document.getElementById('editGuestStatus').value = data.status;
              document.getElementById('editGuestPartnerGivenName').value = data.partnerGivenName;
              document.getElementById('editGuestPartnerFamilyName').value = data.partnerFamilyName;
              document.getElementById('editGuestExpectedAdults').value = data.adults;
              document.getElementById('editGuestExpectedAdultsValue').textContent = data.adults;
              document.getElementById('editGuestAgreedAdults').value = data.adultsAccepted;
              document.getElementById('editGuestAgreedAdultsValue').textContent = data.adultsAccepted;
              document.getElementById('editGuestExpectedChildren').value = data.children;
              document.getElementById('editGuestExpectedChildrenValue').textContent = data.children;
              document.getElementById('editGuestAgreedChildren').value = data.childrenAccepted;
              document.getElementById('editGuestAgreedChildrenValue').textContent = data.childrenAccepted;
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
    const actionPath = GikDefTmpl.editGuest.form.attributes.getNamedItem('action').value + guestId;
    const hdrs = GikDefTmpl.newXhrHeaders();

    fetch(actionPath, {
              method: 'POST',
              headers: hdrs,
              body: formData
          })
    .then(response => response.json())
    .then(data => {
            console.log(data);
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

        GikDefTmpl.editGuest.form = document.getElementById('editGuestForm');
        GikDefTmpl.editGuest.form.addEventListener('submit', (e) => { e.preventDefault(); GikDefTmpl.editGuest.exec(); });

        GikDefTmpl.editGuest.button = document.getElementById('editGuestButton');
        GikDefTmpl.editGuest.button.addEventListener('click', GikDefTmpl.editGuest.exec);
    }
}

GikDefTmpl.editGuest.init();
