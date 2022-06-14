/*
 * SPDX-FileCopyrightText: (C) 2022 Matthias Fehring / www.huessenbergnetz.de
 * SPDX-License-Identifier: AGPL-3.0-or-later
 */

var GikDefTmpl = GikDefTmpl || {};

GikDefTmpl.Guest = GikDefTmpl.Guest || {};

GikDefTmpl.Guest.Invite = GikDefTmpl.Guest.Invite || {};

GikDefTmpl.Guest.Invite.modal = null;

GikDefTmpl.Guest.Invite.form = null;

GikDefTmpl.Guest.Invite.buttons = null;

GikDefTmpl.Guest.Invite.subject = null;

GikDefTmpl.Guest.Invite.subjectInput = null;

GikDefTmpl.Guest.Invite.copySubjectBtn = null;

GikDefTmpl.Guest.Invite.copyTextBtn = null;

GikDefTmpl.Guest.Invite.textArea = null;

GikDefTmpl.Guest.Invite.loadData = function(event) {
    const btn = event.target;
    const eventId = btn.dataset.event;
    const guestId = btn.dataset.guest;
    const method = btn.dataset.method;
    const action = '/controlcenter/events/' + eventId + '/guests/invite/' + guestId + '/' + method;
    const hdrs = GikDefTmpl.newXhrHeaders();

    fetch(action, {
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
              if (data.subject) {
                  GikDefTmpl.Guest.Invite.subjectInput.value= data.subject;
                  GikDefTmpl.Guest.Invite.subject.style.display = 'flex';
              } else {
                  GikDefTmpl.Guest.Invite.subject.style.display = 'none';
              }

              GikDefTmpl.Guest.Invite.textArea.value = data.text;

              GikDefTmpl.Guest.Invite.form.action = action;

              GikDefTmpl.Guest.Invite.modal.show();
          })
    .catch(error => {
              if (error instanceof Response) {
                  error.json().then(json => {
                                        GikDefTmpl.showError(json.error.title, json.error.text);
                                    });
              } else {
                  GikDefTmpl.showError(error.name, error.message);
              }
          });
}

GikDefTmpl.Guest.Invite.exec = function(event) {
    const action = GikDefTmpl.Guest.Invite.form.action;
    const method = action.slice(-1);
    const hdrs = GikDefTmpl.newXhrHeaders();
    const formData = new FormData(GikDefTmpl.Guest.Invite.form);

    fetch(action, {
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
              const btn = tr.querySelector('button[data-method="' + method + '"]')
              btn.classList.replace('btn-outline-secondary', 'btn-outline-success')
          })
    .catch(error => {
               if (error instanceof Response) {
                   error.json().then(json => {
                                         GikDefTmpl.showError(json.error.title, json.error.text);
                                     });
               } else {
                   GikDefTmpl.showError(error.name, error.message);
               }
           })
    .finally(() => {
                 GikDefTmpl.Guest.Invite.modal.hide();
             });
}

GikDefTmpl.Guest.Invite.init = function() {
    const igm = document.getElementById('inviteGuestModal');
    if (igm) {
        GikDefTmpl.Guest.Invite.modal = bootstrap.Modal.getOrCreateInstance(igm);

        GikDefTmpl.Guest.Invite.buttons = document.getElementsByClassName('invite-guest-btn');
        for (let i = 0; i < GikDefTmpl.Guest.Invite.buttons.length; ++i) {
            const btn = GikDefTmpl.Guest.Invite.buttons.item(i);
            btn.addEventListener('click', GikDefTmpl.Guest.Invite.loadData);
        }

        GikDefTmpl.Guest.Invite.form = document.forms['inviteGuestForm'];
        GikDefTmpl.Guest.Invite.form.addEventListener('submit', (e) => { e.preventDefault(); GikDefTmpl.Guest.Invite.exec(e); });

        GikDefTmpl.Guest.Invite.subject = document.getElementById('inviteGuestSubject');
        GikDefTmpl.Guest.Invite.subjectInput = document.getElementById('inviteGuestSubjectInput');
        GikDefTmpl.Guest.Invite.copySubjectBtn = document.getElementById('inviteGuestSubjectCopy');
        GikDefTmpl.Guest.Invite.copySubjectBtn.addEventListener('click', () => {
            GikDefTmpl.Guest.Invite.subjectInput.select();
            GikDefTmpl.Guest.Invite.subjectInput.setSelectionRange(0, 99999);
            navigator.clipboard.writeText(GikDefTmpl.Guest.Invite.subjectInput.value);
        });

        GikDefTmpl.Guest.Invite.textArea = document.getElementById('inviteGuestText');
        GikDefTmpl.Guest.Invite.copyTextBtn = document.getElementById('inviteGuestTextCopy');
        GikDefTmpl.Guest.Invite.copyTextBtn.addEventListener('click', () => {
            GikDefTmpl.Guest.Invite.textArea.select();
            GikDefTmpl.Guest.Invite.textArea.setSelectionRange(0, 99999);
            navigator.clipboard.writeText(GikDefTmpl.Guest.Invite.textArea.value);
        });
    }
}

GikDefTmpl.Guest.Invite.init();
