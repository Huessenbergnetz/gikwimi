/*
 * SPDX-FileCopyrightText: (C) 2022 Matthias Fehring / www.huessenbergnetz.de
 * SPDX-License-Identifier: AGPL-3.0-or-later
 */

var GikDefTmpl = GikDefTmpl || {};

GikDefTmpl.Group = GikDefTmpl.Group || {};

GikDefTmpl.Group.Add = GikDefTmpl.Group.Add || {};

GikDefTmpl.Group.Add.modal = null;

GikDefTmpl.Group.Add.form = null;

GikDefTmpl.Group.Add.button = null;

GikDefTmpl.Group.Add.exec = function(event) {
    const submitButton = event.submitter;
    GikDefTmpl.switchButton(submitButton);

    GikDefTmpl.resetFormFieldErrors(GikDefTmpl.Group.Add.form);

    let fd = new FormData(GikDefTmpl.Group.Add.form);
    const actionPath = GikDefTmpl.Group.Add.form.action;
    const hdrs = GikDefTmpl.newXhrHeaders();

    fetch(actionPath, {
              method: 'POST',
              headers: hdrs,
              body: fd
          })
    .then(response => {
              if (response.ok) {
                  return response.json();
              } else {
                  return Promise.reject(response);
              }
          })
    .then(group => {
              const template = document.getElementById('group-template');
              const clone = template.content.cloneNode(true);

              const section = clone.querySelector('section');
              section.id = 'group' + group.id;

              const header = clone.querySelector('h2');
              header.textContent = group.name;

              const button = clone.querySelector('button');
              button.value = group.id;

              document.getElementById('guestgroups').appendChild(clone);

              GikDefTmpl.Group.Add.form.reset();
              GikDefTmpl.Group.Add.modal.hide();
          })
    .catch(error => {
               if (error instanceof Response) {
                   error.json().then(json => {
                                         if (json.error) {
                                             GikDefTmpl.Group.Add.modal.hide();
                                             GikDefTmpl.showError(json.error.title, json.error.text);
                                         } else if (json.fielderrors) {
                                             GikDefTmpl.setFormFieldErrors(GikDefTmpl.Group.Add.form, json.fielderrors);
                                         }
                                     });
               } else {
                   GikDefTmpl.Group.Add.modal.hide();
                   GikDefTmpl.showError(error.name, error.message);
               }
           })
    .finally(() => {
                 GikDefTmpl.switchButton(submitButton);
             });
}

GikDefTmpl.Group.Add.init = function() {
    const agm = document.getElementById('addGroupModal');
    if (agm) {
        GikDefTmpl.Group.Add.modal = bootstrap.Modal.getOrCreateInstance(agm);

        GikDefTmpl.Group.Add.form = document.forms['addGroupForm'];
        GikDefTmpl.Group.Add.form.addEventListener('submit', (e) => { e.preventDefault(); GikDefTmpl.Group.Add.exec(e); });
    }
}

GikDefTmpl.Group.Add.init();
