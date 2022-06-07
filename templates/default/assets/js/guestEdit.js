/*
 * SPDX-FileCopyrightText: (C) 2022 Matthias Fehring / www.huessenbergnetz.de
 * SPDX-License-Identifier: AGPL-3.0-or-later
 */

var GikDefTmpl = GikDefTmpl || {};

GikDefTmpl.Guest = GikDefTmpl.Guest || {};

GikDefTmpl.Guest.Edit = GikDefTmpl.Guest.Edit || {};

GikDefTmpl.Guest.Edit.modal = null;

GikDefTmpl.Guest.Edit.form = null;

GikDefTmpl.Guest.Edit.button = null;

GikDefTmpl.Guest.Edit.buttons = null;

GikDefTmpl.Guest.Edit.loadData = function(event) {
    const button = event.target;
    GikDefTmpl.switchButton(button);
    const guestId = button.value;
    const hdrs = GikDefTmpl.newXhrHeaders();
    const actionPath = GikDefTmpl.Guest.Edit.form.action + guestId;
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

              const els = GikDefTmpl.Guest.Edit.form.elements;

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

              GikDefTmpl.resetFormFieldErrors(GikDefTmpl.Guest.Edit.form);
              GikDefTmpl.Guest.Edit.modal.show();
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

GikDefTmpl.Guest.Edit.exec = function(event) {
    const button = event.submitter;
    GikDefTmpl.switchButton(button);

    GikDefTmpl.resetFormFieldErrors(GikDefTmpl.Guest.Edit.form);

    const formData = new FormData(GikDefTmpl.Guest.Edit.form);
    const guestId = document.getElementById('editGuestId').value;
    const actionPath = GikDefTmpl.Guest.Edit.form.action + guestId;
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
              GikDefTmpl.Guest.setStatusIcon(statusIcon, guest.status);

              const countSpans = countTd.getElementsByTagName('span');
              countSpans[1].textContent = guest.adultsAccepted;
              countSpans[2].textContent = guest.adults;
              countSpans[4].textContent = guest.childrenAccepted;
              countSpans[5].textContent = guest.children;

              GikDefTmpl.Guest.Edit.modal.hide();
          })
    .catch(error => {
               if (error instanceof Response) {
                   error.json().then(json => {
                                         if (json.error) {
                                             GikDefTmpl.Guest.Edit.modal.hide();
                                             GikDefTmpl.showError(json.error.title, json.error.text);
                                         } else if (json.fielderrors) {
                                             GikDefTmpl.setFormFieldErrors(GikDefTmpl.Guest.Edit.form, json.fielderrors);
                                         }
                                     });
               } else {
                   GikDefTmpl.Guest.Edit.modal.hide();
                   GikDefTmpl.showError(error.name, error.message);
               }
           })
    .finally(() => {
                 GikDefTmpl.switchButton(button);
             });
}

GikDefTmpl.Guest.Edit.init = function() {
    const egm = document.getElementById('editGuestModal');
    if (egm) {
        GikDefTmpl.Guest.Edit.modal = bootstrap.Modal.getOrCreateInstance(egm);

        GikDefTmpl.Guest.Edit.buttons = document.getElementsByClassName('edit-guest-btn');
        for (let i = 0; i < GikDefTmpl.Guest.Edit.buttons.length; ++i) {
            const btn = GikDefTmpl.Guest.Edit.buttons.item(i);
            btn.addEventListener('click', GikDefTmpl.Guest.Edit.loadData);
        }

        GikDefTmpl.Guest.Edit.form = document.forms['editGuestForm'];
        GikDefTmpl.Guest.Edit.form.addEventListener('submit', (e) => { e.preventDefault(); GikDefTmpl.Guest.Edit.exec(e); });
    }
}

GikDefTmpl.Guest.Edit.init();
