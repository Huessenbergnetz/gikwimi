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
    GikDefTmpl.switchButton(button);
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
                                         GikDefTmpl.showError(json.error.title, json.error.text);
                                     })
               } else {
                   GikDefTmpl.showError(error.name, error.message);
               }
           })
    .finally(() => {
                 GikDefTmpl.switchButton(button);
            });
}

GikDefTmpl.editGuest.exec = function() {
    const button = document.getElementById('editGuestSubmitButton');
    GikDefTmpl.switchButton(button);

    for (let i = 0; i < GikDefTmpl.editGuest.form.elements.length; ++i) {
            GikDefTmpl.editGuest.form.elements[i].classList.remove('is-invalid');
        }

    const formData = new FormData(GikDefTmpl.editGuest.form);
    const guestId = document.getElementById('editGuestId').value;
    const actionPath = GikDefTmpl.editGuest.form.action + guestId;
    const hdrs = GikDefTmpl.newXhrHeaders();

    fetch(actionPath, {
              method: 'POST',
              headers: hdrs,
              body: formData
          })
    .then(response => {
              if (response.ok) {
                  return response.json();
              } else {
                  return Promise.reject(response);
              }
          })
    .then(guest => {
              const tr = document.getElementById('guest-' + guest.id);
              const tds = tr.getElementsByTagName('td');

              const nameTd      = tds[1];
              const statusTd    = tds[2];
              const countTd     = tds[3];

              if (guest.contact.addressee.familyName === guest.partnerFamilyName) {
                  nameTd.textContent = guest.contact.addressee.givenName + ' and ' + guest.partnerGivenName + ' ' + guest.contact.addressee.familyName;
              } else {
                  nameTd.innerHTML = guest.contact.addressee.givenName + ' ' + guest.contact.addressee.familyName + '<br>' + guest.partnerGivenName + ' ' + guest.partnerFamilyName;
              }

              const statusIcon = statusTd.getElementsByTagName('i')[0];
              GikDefTmpl.setGuestStatusIcon(statusIcon, guest.status);

              const countSpans = countTd.getElementsByTagName('span');
              countSpans[1].textContent = guest.adultsAccepted;
              countSpans[2].textContent = guest.adults;
              countSpans[4].textContent = guest.childrenAccepted;
              countSpans[5].textContent = guest.children;

              GikDefTmpl.editGuest.modal.hide();
          })
    .catch(error => {
               if (error instanceof Response) {
                   error.json().then(json => {
                                         if (json.error) {
                                             GikDefTmpl.editGuest.modal.hide();
                                             GikDefTmpl.showError(json.error.title, json.error.text);
                                         } else if (json.fielderrors) {
                                             const fe = json.fielderrors;
                                             const els = GikDefTmpl.editGuest.form.elements;
                                             for (const field in fe) {
                                                 if (fe.hasOwnProperty(field)) {
                                                     const el = els.namedItem(field);
                                                     const fb = el.nextElementSibling;
                                                     fb.innerHTML = fe[field].join('<br>');
                                                     el.classList.add('is-invalid');
                                                 }
                                             }
                                         }
                                     });
               } else {
                   GikDefTmpl.editGuest.modal.hide();
                   GikDefTmpl.showError(error.name, error.message);
               }
           })
    .finally(() => {
                 GikDefTmpl.switchButton(button);
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
    }
}

GikDefTmpl.editGuest.init();
