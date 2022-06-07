/*
 * SPDX-FileCopyrightText: (C) 2022 Matthias Fehring / www.huessenbergnetz.de
 * SPDX-License-Identifier: AGPL-3.0-or-later
 */

var GikDefTmpl = GikDefTmpl || {};

GikDefTmpl.Guest = GikDefTmpl.Guest || {};

GikDefTmpl.Guest.Remove = GikDefTmpl.Guest.Remove || {};

GikDefTmpl.Guest.Remove.modal = null;

GikDefTmpl.Guest.Remove.buttons = null;

GikDefTmpl.Guest.Remove.form = null;

GikDefTmpl.Guest.Remove.loadData = function(event) {
    const guestId = event.target.dataset.value;
    const eventId = document.getElementById('removeGuestEventId').value;
    const actionPath = '/controlcenter/events/' + eventId + '/guests/remove/' + guestId;
    const hdrs = GikDefTmpl.newXhrHeaders();

    GikDefTmpl.Guest.Remove.form.action = actionPath;

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
    .then(guest => {
              document.getElementById('removeGuestQuestion').textContent = 'Are you sure you want to remove the guest “' + guest.contact.addressee.formattedName + '” from the group “' + guest.group.name + '”?';
              document.getElementById('removeGuestName').textContent = guest.contact.addressee.formattedName;
              GikDefTmpl.Guest.Remove.modal.show();
          })
    .catch(error => {
               if (error instanceof Response) {
                   error.json().then(json => {GikDefTmpl.showError(json.error.title, json.error.text);});
               } else {
                   GikDefTmpl.showError(error.name, error.message);
               }
           });
}

GikDefTmpl.Guest.Remove.exec = function(event) {
    const button = event.submitter;
    GikDefTmpl.switchButton(button);

    const actionPath = GikDefTmpl.Guest.Remove.form.action;
    const formData = new FormData(GikDefTmpl.Guest.Remove.form);
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
              tr.remove();
              GikDefTmpl.Guest.Remove.modal.hide();
          })
    .catch(error => {
               if (error instanceof Response) {
                   error.json().then(json => {
                                         if (json.error) {
                                             GikDefTmpl.Guest.Remove.modal.hide();
                                             GikDefTmpl.showError(json.error.title, json.error.text);
                                         }
                                     });
               } else {
                   GikDefTmpl.Guest.Remove.modal.hide();
                   GikDefTmpl.showError(error.name, error.message);
               }
           })
    .finally(() => {
                 GikDefTmpl.switchButton(button);
             });
}

GikDefTmpl.Guest.Remove.init = function() {
    const rgm = document.getElementById('removeGuestModal');
    if (rgm) {
        GikDefTmpl.Guest.Remove.modal = bootstrap.Modal.getOrCreateInstance(rgm);

        GikDefTmpl.Guest.Remove.buttons = document.getElementsByClassName('delete-guest-btn');
        for (let i = 0; i < GikDefTmpl.Guest.Remove.buttons.length; ++i) {
            const btn = GikDefTmpl.Guest.Remove.buttons.item(i);
            btn.addEventListener('click', (e) => {e.preventDefault(); GikDefTmpl.Guest.Remove.loadData(e)});
        }

        GikDefTmpl.Guest.Remove.form = document.forms['removeGuestForm'];
        GikDefTmpl.Guest.Remove.form.addEventListener('submit', (e) => { e.preventDefault(); GikDefTmpl.Guest.Remove.exec(e); });
    }
}

GikDefTmpl.Guest.Remove.init();
