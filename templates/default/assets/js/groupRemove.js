/*
 * SPDX-FileCopyrightText: (C) 2022 Matthias Fehring / www.huessenbergnetz.de
 * SPDX-License-Identifier: AGPL-3.0-or-later
 */

var GikDefTmpl = GikDefTmpl || {};

GikDefTmpl.Group = GikDefTmpl.Group || {};

GikDefTmpl.Group.Remove = GikDefTmpl.Group.Remove || {};

GikDefTmpl.Group.Remove.modal = null;

GikDefTmpl.Group.Remove.buttons = null;

GikDefTmpl.Group.Remove.form = null;

GikDefTmpl.Group.Remove.loadData = function(event) {
    const btn = event.target;
    GikDefTmpl.switchButton(btn);
    const groupId = btn.value;
    const action  = GikDefTmpl.Group.Remove.form.action + groupId;
    const hdrs    = GikDefTmpl.newXhrHeaders();

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
    .then(group => {
              document.getElementById('removeGroupId').value = group.id;
              document.getElementById('removeGroupName').textContent = group.name;
              document.getElementById('removeGroupHint1').textContent = 'Are you absolutely sure? This action cannot be undone. This will permanently delete the group “' + group.name + '” and all associated guests.'
              document.getElementById('removeGroupHint2').textContent = 'Please type “' + group.name + '” to confirm.';
              const nameInput = GikDefTmpl.Group.Remove.form['name']
              nameInput.pattern = group.name;
              nameInput.value = '';

              GikDefTmpl.Group.Remove.modal.show();
          })
    .catch(error => {
               if (error instanceof Response) {
                   error.json().then(json => { GikDefTmpl.showError(json.error.title, json.error.text); });
               } else {
                   GikDefTmpl.showError(error.name, error.message);
               }
           })
    .finally(() => {
                 GikDefTmpl.switchButton(btn);
             });
}

GikDefTmpl.Group.Remove.exec = function(event) {
    const btn      = event.submitter;
    GikDefTmpl.switchButton(btn);

    const groupId  = document.getElementById('removeGroupId').value;
    const action   = GikDefTmpl.Group.Remove.form.action + groupId;
    const hdrs     = GikDefTmpl.newXhrHeaders();
    const formData = new FormData(GikDefTmpl.Group.Remove.form);

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
    .then(group => {
              const groupSection = document.getElementById('group-' + group.id);
              groupSection.remove();
              GikDefTmpl.Group.Remove.modal.hide();

          })
    .catch(error => {
               if (error instanceof Response) {
                   error.json().then(json => {
                                         if (json.error) {
                                             GikDefTmpl.Group.Remove.modal.hide();
                                             GikDefTmpl.showError(json.error.title, json.error.text);
                                         } else if (json.fielderrors) {
                                             GikDefTmpl.setFormFieldErrors(GikDefTmpl.Group.Remove.form, json.fielderrors);
                                         }
                                     });
               } else {
                   GikDefTmpl.Group.Remove.modal.hide();
                   GikDefTmpl.showError(error.name, error.message);
               }
           })
    .finally(() => {
                 GikDefTmpl.switchButton(btn);
             });
}

GikDefTmpl.Group.Remove.init = function() {
    const rgm = document.getElementById('removeGroupModal');
    if (rgm) {
        GikDefTmpl.Group.Remove.modal = bootstrap.Modal.getOrCreateInstance(rgm);

        GikDefTmpl.Group.Remove.buttons = document.getElementsByClassName('remove-group-btn');
        for (let i = 0; i < GikDefTmpl.Group.Remove.buttons.length; ++i) {
            const btn = GikDefTmpl.Group.Remove.buttons.item(i);
            btn.addEventListener('click', (e) => { e.preventDefault(); GikDefTmpl.Group.Remove.loadData(e); });
        }

        GikDefTmpl.Group.Remove.form = document.forms['removeGroupForm'];
        GikDefTmpl.Group.Remove.form.addEventListener('submit', (e) => { e.preventDefault(); GikDefTmpl.Group.Remove.exec(e); });
    }
}

GikDefTmpl.Group.Remove.init();
