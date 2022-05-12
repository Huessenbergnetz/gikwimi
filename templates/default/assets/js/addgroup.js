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
    let fd = new FormData(GikDefTmpl.addGroup.form);
    const actionPath = GikDefTmpl.addGroup.form.attributes.getNamedItem('action').value;
    const hdrs = GikDefTmpl.newXhrHeaders();

    fetch(actionPath, {
              method: 'POST',
              headers: hdrs,
              body: fd
          })
    .then(response => response.json())
    .then(data => {
              let template = document.getElementById('group-template');
              let clone = template.content.cloneNode(true);

              let section = clone.querySelector('section');
              section.id = 'group_' + data.id;

              let header = clone.querySelector('h2');
              header.textContent = data.name;

              let button = clone.querySelector('button');
              button.value = data.id;

              document.getElementById('guestgroups').appendChild(clone);
          })
    .catch(console.error);

    GikDefTmpl.addGroup.modal.hide();
}

GikDefTmpl.addGroup.init = function() {
    const agm = document.getElementById('addGroupModal');
    if (agm) {
        GikDefTmpl.addGroup.modal = bootstrap.Modal.getOrCreateInstance(agm);

        GikDefTmpl.addGroup.form = document.getElementById('addGroupForm');
        GikDefTmpl.addGroup.form.addEventListener('submit', (e) => { e.preventDefault(); GikDefTmpl.addGroup.exec(); });

        GikDefTmpl.addGroup.button = document.getElementById('createGroupButton');
        GikDefTmpl.addGroup.button.addEventListener('click', GikDefTmpl.addGroup.exec);
    }
}

GikDefTmpl.addGroup.init();
