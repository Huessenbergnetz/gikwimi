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
    const guestPartnerGivenName = document.getElementById('guestPartnerGivenName');
    guestPartnerGivenName.value = '';

    const guestPartnerFamilyName = document.getElementById('guestPartnerFamilyName');
    guestPartnerFamilyName.value = '';

    const expectedAdultsRange = document.getElementById('expectedAdultsRange');
    expectedAdultsRange.value = 1;

    const expectedAdultsValue = document.getElementById('expectedAdultsValue');
    expectedAdultsValue.textContent = 1;

    const expectedChildrenRange = document.getElementById('expectedChildrenRange');
    expectedChildrenRange.value = 0;

    const expectedChildrenValue = document.getElementById('expectedChildrenValue');
    expectedChildrenValue.textContent = 0;
}

GikDefTmpl.addGuest.exec = function() {
    const fd = new FormData(GikDefTmpl.addGuest.form);
    const actionPath = GikDefTmpl.addGuest.form.attributes.getNamedItem('action').value;
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

              const tds = tr.getElementsByTagName('td');
              const nameTd      = tds[0];
              const countTd     = tds[1];
              const addressTd   = tds[2];

              if (data.contact.addressee.familyName === data.partnerFamilyName) {
                  nameTd.textContent = data.contact.addressee.givenName + ' and ' + data.partnerGivenName + ' ' + data.contact.addressee.familyName;
              } else {
                  nameTd.innerHTML = data.contact.addressee.givenName + ' ' + data.contact.addressee.familyName + '<br>' + data.partnerGivenName + ' ' + data.partnerFamilyName;
              }

              const countSpans = countTd.getElementsByTagName('span');
              countSpans[1].textContent = data.adultsAccepted;
              countSpans[2].textContent = data.adults;
              countSpans[4].textContent = data.childrenAccepted;
              countSpans[5].textContent = data.children;

              if (data.contact.addressee.addresses && data.contact.addressee.addresses.length) {
                  const address = data.contact.addressee.addresses[0];
                  let addressHtml = '';
                  if (address.street) {
                      addressHtml += address.street + '<br>';
                  }
                  if (address.postalCode) {
                      addressHtml += address.postalCode + ' ';
                  }
                  if (address.locality) {
                      addressHtml += address.locality;
                  }
                  if (address.locality || address.postalCode) {
                      addressHtml += '<br>';
                  }
                  addressHtml += address.region;
                  if (address.region && address.country) {
                      addressHtml += ', ' + address.country;
                  }
                  addressTd.innerHTML = addressHtml;
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

GikDefTmpl.addGuest.addRangeEventListeners = function() {
    const expectedAdultsRange = document.getElementById('expectedAdultsRange');
    const expectedAdultsValue = document.getElementById('expectedAdultsValue');
    expectedAdultsRange.addEventListener('input', (e) => {expectedAdultsValue.textContent = e.target.value;} )

    const expectedChildrenRange = document.getElementById('expectedChildrenRange');
    const expectedChildrenValue = document.getElementById('expectedChildrenValue');
    expectedChildrenRange.addEventListener('input', (e) => {expectedChildrenValue.textContent = e.target.value});
}

GikDefTmpl.addGuest.init = function() {
    const agm = document.getElementById('addGuestModal');
    if (agm) {
        GikDefTmpl.addGuest.modal = bootstrap.Modal.getOrCreateInstance(agm);
        agm.addEventListener('show.bs.modal', GikDefTmpl.addGuest.loadData);

        GikDefTmpl.addGuest.form = document.getElementById('addGuestForm');
        GikDefTmpl.addGuest.form.addEventListener('submit', (e) => { e.preventDefault(); GikDefTmpl.addGuest.exec(); });

        GikDefTmpl.addGuest.button = document.getElementById('addGuestButton');
        GikDefTmpl.addGuest.button.addEventListener('click', GikDefTmpl.addGuest.exec);

        const addressbooks = document.getElementById('addGuestFromAddressbook');
        addressbooks.addEventListener('change', function(e) {
            GikDefTmpl.addGuest.loadContacts(e.target.value);
        })

        GikDefTmpl.addGuest.addRangeEventListeners();
    }
}

GikDefTmpl.addGuest.init();
