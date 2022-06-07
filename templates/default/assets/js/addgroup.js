/*
 * SPDX-FileCopyrightText: (C) 2022 Matthias Fehring / www.huessenbergnetz.de
 * SPDX-License-Identifier: AGPL-3.0-or-later
 */

var GikDefTmpl = GikDefTmpl || {};

GikDefTmpl.addGroup = GikDefTmpl.addGroup || {};

GikDefTmpl.addGroup.modal = null;

GikDefTmpl.addGroup.form = null;

GikDefTmpl.addGroup.button = null;

GikDefTmpl.addGroup.exec = function() {
    const submitButton = document.getElementById('createGroupButton');
    GikDefTmpl.switchButton(submitButton);

    for (let i = 0; i < GikDefTmpl.addGroup.form.elements.length; ++i) {
        GikDefTmpl.addGroup.form.elements[i].classList.remove('is-invalid');
    }

    let fd = new FormData(GikDefTmpl.addGroup.form);
    const actionPath = GikDefTmpl.addGroup.form.action;
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
              section.id = 'group_' + group.id;

              const header = clone.querySelector('h2');
              header.textContent = group.name;

              const button = clone.querySelector('button');
              button.value = group.id;

              document.getElementById('guestgroups').appendChild(clone);

              GikDefTmpl.addGroup.form.reset();
              GikDefTmpl.addGroup.modal.hide();
          })
    .catch(error => {
               if (error instanceof Response) {
                   error.json().then(json => {
                                         if (json.error) {
                                             GikDefTmpl.addGroup.modal.hide();
                                             GikDefTmpl.showError(json.error.title, json.error.text);
                                         } else if (json.fielderrors) {
                                             const fe = json.fielderrors;
                                             const els = GikDefTmpl.addGroup.form.elements;
                                             for (const field in fe) {
                                                 if (fe.hasOwnProperty(field)) {
                                                     const el = els.namedItem(field)
                                                     const fb = el.nextElementSibling;
                                                     fb.innerHTML = fe[field].join('<br>');
                                                     el.classList.add('is-invalid');
                                                 }
                                             }
                                         }
                                     });
               } else {
                   GikDefTmpl.addGroup.modal.hide();
                   GikDefTmpl.showError(error.name, error.message);
               }
           })
    .finally(() => {
                 GikDefTmpl.switchButton(submitButton);
             });
}

GikDefTmpl.addGroup.init = function() {
    const agm = document.getElementById('addGroupModal');
    if (agm) {
        GikDefTmpl.addGroup.modal = bootstrap.Modal.getOrCreateInstance(agm);

        GikDefTmpl.addGroup.form = document.forms['addGroupForm'];
        GikDefTmpl.addGroup.form.addEventListener('submit', (e) => { e.preventDefault(); GikDefTmpl.addGroup.exec(); });
    }
}

GikDefTmpl.addGroup.init();
