/*
 * SPDX-FileCopyrightText: (C) 2022 Matthias Fehring / www.huessenbergnetz.de
 * SPDX-License-Identifier: AGPL-3.0-or-later
 */

var GikDefTmpl = GikDefTmpl || {};

GikDefTmpl.addGuest = GikDefTmpl.addGuest || {};

GikDefTmpl.addGuest.modal = null;

GikDefTmpl.addGuest.form = null;

GikDefTmpl.addGuest.button = null;

GikDefTmpl.addGuest.addressBooksLoaded = false;

GikDefTmpl.addGuest.resetForm = function() {
    const els = GikDefTmpl.addGuest.form.elements;

    els.namedItem('partnerGivenName').value = '';
    els.namedItem('partnerFamilyName').value = '';

    const e = new Event('input');

    const ea = els.namedItem('expectedAdults');
    ea.value = 1;
    ea.dispatchEvent(e);

    const ec = els.namedItem('expectedChildren');
    ec.value = 0;
    ec.dispatchEvent(e);
}

GikDefTmpl.addGuest.exec = function() {
    const fd = new FormData(GikDefTmpl.addGuest.form);
    const actionPath = GikDefTmpl.addGuest.form.action;
    const hdrs = GikDefTmpl.newXhrHeaders();

    fetch(actionPath, {
              method: 'POST',
              headers: hdrs,
              body: fd
          })
    .then(response => response.json())
    .then(data => {
              const template = document.getElementById('guest-row-template');
              const clone = template.content.cloneNode(true);

              const tr = clone.querySelector('tr');
              tr.id = data.id;

              const tds = tr.getElementsByTagName('td');
              const buttonTd    = tds[0];
              const nameTd      = tds[1];
              const statusTd    = tds[2]
              const countTd     = tds[3];
              const addressTd   = tds[4];
              const inviteTd    = tds[5];

              const editGuestBtn = buttonTd.getElementsByClassName('edit-guest-btn')[0];
              editGuestBtn.value = data.id;
              editGuestBtn.addEventListener('click', GikDefTmpl.editGuest.loadData);
              buttonTd.getElementsByClassName('invite-guest-btn')[0].href = '/i/' + data.uid;
              buttonTd.getElementsByClassName('delete-guest-btn')[0].dataset.value = data.id;

              if (data.contact.addressee.familyName === data.partnerFamilyName) {
                  nameTd.textContent = data.contact.addressee.givenName + ' and ' + data.partnerGivenName + ' ' + data.contact.addressee.familyName;
              } else {
                  nameTd.innerHTML = data.contact.addressee.givenName + ' ' + data.contact.addressee.familyName + '<br>' + data.partnerGivenName + ' ' + data.partnerFamilyName;
              }

              const statusIcon = statusTd.getElementsByTagName('i')[0];
              if (data.status === 1) { // Agreed
                  statusIcon.className = 'bi bi-check-circle text-success';
              } else if (data.status === 2) { // Canceled
                  statusIcon.className = 'bi bi-x-circle text-warning';
              } else { // DefaultStatus
                  statusIcon.className = 'bi bi-dash-circle text-secondary';
              }

              const countSpans = countTd.getElementsByTagName('span');
              countSpans[1].textContent = data.adultsAccepted;
              countSpans[2].textContent = data.adults;
              countSpans[4].textContent = data.childrenAccepted;
              countSpans[5].textContent = data.children;

              if (data.contact.addressee.addresses && data.contact.addressee.addresses.length) {
                  addressTd.innerHTML = GikDefTmpl.composeAddress(data.contact.addressee.addresses[0]);
              }

              const section = document.getElementById('group_' + data.group.id).getElementsByTagName('tbody')[0].appendChild(clone);

              GikDefTmpl.addGuest.resetForm();
          })
    .catch(console.error);

    GikDefTmpl.addGuest.modal.hide();
}

GikDefTmpl.addGuest.loadContacts = function(addressbookId) {
    const addressbooks = document.getElementById('addGuestFromAddressbook');
    addressbooks.disabled = true;
    const hdrs = GikDefTmpl.newXhrHeaders();
    fetch('/controlcenter/addressbooks/' + addressbookId + '/contacts', {
              headers: hdrs
          })
    .then(response => response.json())
    .then(data => {
              const contacts = document.getElementById('selectGuest');

              while (contacts.lastChild) {
                  contacts.removeChild(contacts.lastChild)
              }

              const firstOpt = document.createElement('option');
              firstOpt.value = 0;
              firstOpt.textContent = 'Select a guest';
              contacts.appendChild(firstOpt);

              for (const cont of data) {
                  const opt = document.createElement('option');
                  opt.value = cont.id;
                  opt.textContent = cont.addressee.formattedName;
                  contacts.appendChild(opt);
              }

              addressbooks.disabled = false;
          })
    .catch(console.error);
}

GikDefTmpl.addGuest.loadData = function(e) {
    document.getElementById('addGuestGuestGroupId').value = e.relatedTarget.value;

    if (!GikDefTmpl.addGuest.addressBooksLoaded) {
        const hdrs = GikDefTmpl.newXhrHeaders();
        fetch('/controlcenter/addressbooks', {
                  headers: hdrs
              })
        .then(response => response.json())
        .then(data => {
                  const addressbooks = document.getElementById('addGuestFromAddressbook');
                  for (const book of data) {
                      const opt = document.createElement('option');
                      opt.value = book.id;
                      opt.textContent = book.name;
                      addressbooks.appendChild(opt);
                  }
                  GikDefTmpl.addGuest.loadContacts(addressbooks.value);
                  GikDefTmpl.addGuest.addressBooksLoaded = true;
              })
        .catch(console.error);
    }
}

GikDefTmpl.addGuest.init = function() {
    const agm = document.getElementById('addGuestModal');
    if (agm) {
        GikDefTmpl.addGuest.modal = bootstrap.Modal.getOrCreateInstance(agm);
        agm.addEventListener('show.bs.modal', GikDefTmpl.addGuest.loadData);

        GikDefTmpl.addGuest.form = document.forms['addGuestForm'];
        GikDefTmpl.addGuest.form.addEventListener('submit', (e) => { e.preventDefault(); GikDefTmpl.addGuest.exec(); });

        GikDefTmpl.addGuest.button = document.getElementById('addGuestButton');
        GikDefTmpl.addGuest.button.addEventListener('click', GikDefTmpl.addGuest.exec);

        const addressbooks = document.getElementById('addGuestFromAddressbook');
        addressbooks.addEventListener('change', function(e) {
            GikDefTmpl.addGuest.loadContacts(e.target.value);
        })
    }
}

GikDefTmpl.addGuest.init();
