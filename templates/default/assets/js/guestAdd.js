/*
 * SPDX-FileCopyrightText: (C) 2022 Matthias Fehring / www.huessenbergnetz.de
 * SPDX-License-Identifier: AGPL-3.0-or-later
 */

var GikDefTmpl = GikDefTmpl || {};

GikDefTmpl.Guest = GikDefTmpl.Guest || {};

GikDefTmpl.Guest.Add = GikDefTmpl.Guest.Add || {};

GikDefTmpl.Guest.Add.modal = null;

GikDefTmpl.Guest.Add.form = null;

GikDefTmpl.Guest.Add.button = null;

GikDefTmpl.Guest.Add.addressBooksLoaded = false;

GikDefTmpl.Guest.Add.resetForm = function() {
    const els = GikDefTmpl.Guest.Add.form.elements;

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

GikDefTmpl.Guest.Add.exec = function(event) {
    const button = event.submitter;
    GikDefTmpl.switchButton(button);

    GikDefTmpl.resetFormFieldErrors(GikDefTmpl.Guest.Add.form);

    const fd = new FormData(GikDefTmpl.Guest.Add.form);
    const actionPath = GikDefTmpl.Guest.Add.form.action;
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
    .then(data => {
              const template = document.getElementById('guest-row-template');
              const clone = template.content.cloneNode(true);

              const tr = clone.querySelector('tr');
              tr.id = 'guest-' + data.id;

              const tds = tr.getElementsByTagName('td');
              const buttonTd    = tds[0];
              const nameTd      = tds[1];
              const statusTd    = tds[2]
              const countTd     = tds[3];
              const addressTd   = tds[4];
              const inviteTd    = tds[5];

              const editGuestBtn = buttonTd.getElementsByClassName('edit-guest-btn')[0];
              editGuestBtn.value = data.id;
              editGuestBtn.addEventListener('click', GikDefTmpl.Guest.Edit.loadData);
              buttonTd.getElementsByClassName('invite-guest-btn')[0].href = '/i/' + data.uid;
              const rmGuestBtn = buttonTd.getElementsByClassName('delete-guest-btn').item(0);
              rmGuestBtn.dataset.value = data.id;
              rmGuestBtn.addEventListener('click', (e) => {e.preventDefault(); GikDefTmpl.Guest.Remove.loadData(e)});

              if (data.contact.addressee.familyName === data.partnerFamilyName) {
                  nameTd.textContent = data.contact.addressee.givenName + ' and ' + data.partnerGivenName + ' ' + data.contact.addressee.familyName;
              } else {
                  nameTd.innerHTML = data.contact.addressee.givenName + ' ' + data.contact.addressee.familyName + '<br>' + data.partnerGivenName + ' ' + data.partnerFamilyName;
              }

              const statusIcon = statusTd.getElementsByTagName('i')[0];
              GikDefTmpl.Guest.setStatusIcon(statusIcon, data.status);

              const countSpans = countTd.getElementsByTagName('span');
              countSpans[1].textContent = data.adultsAccepted;
              countSpans[2].textContent = data.adults;
              countSpans[4].textContent = data.childrenAccepted;
              countSpans[5].textContent = data.children;

              if (data.contact.addressee.addresses && data.contact.addressee.addresses.length) {
                  addressTd.innerHTML = GikDefTmpl.composeAddress(data.contact.addressee.addresses[0]);
              }

              const inviteButtons = inviteTd.getElementsByTagName('button');
              for (let i = 0; i < inviteButtons.length; ++i) {
                  const inviteBtn = inviteButtons.item(i);
                  inviteBtn.dataset.guest = data.id;
                  inviteBtn.addEventListener('click', GikDefTmpl.Guest.Invite.loadData);
              }

              document.getElementById('group-' + data.group.id).getElementsByTagName('tbody')[0].appendChild(clone);

              GikDefTmpl.Guest.updateCount(data.group.id, data.adultsAccepted, data.adults, data.childrenAccepted, data.children);

              GikDefTmpl.Guest.Add.resetForm();
              GikDefTmpl.Guest.Add.modal.hide();
          })
    .catch(error => {
               if (error instanceof Response) {
                   error.json().then(json => {
                                         if (json.error) {
                                             GikDefTmpl.Guest.Add.modal.hide();
                                             GikDefTmpl.showError(json.error.title, json.error.text);
                                         } else if (json.fielderrors) {
                                             GikDefTmpl.setFormFieldErrors(GikDefTmpl.Guest.Add.form, json.fielderrors);
                                         }
                                     });
               } else {
                   GikDefTmpl.Guest.Add.modal.hide();
                   GikDefTmpl.showError(error.name, error.message);
               }
           })
    .finally(() => {
                 GikDefTmpl.switchButton(button);
             });
}

GikDefTmpl.Guest.Add.loadContacts = function(addressbookId) {
    const addressbooks = document.getElementById('addGuestFromAddressbook');
    addressbooks.disabled = true;
    const hdrs = GikDefTmpl.newXhrHeaders();

    fetch('/controlcenter/addressbooks/' + addressbookId + '/contacts', {
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
    .catch(error => {
               if (error instanceof Response) {
                   error.json().then(json => {
                                         if (json.error) {
                                             GikDefTmpl.Guest.Add.modal.hide();
                                             GikDefTmpl.showError(json.error.title, json.error.text);
                                         }
                                     });
               } else {
                   GikDefTmpl.Guest.Add.modal.hide();
                   GikDefTmpl.showError(error.name, error.message);
               }
           });
}

GikDefTmpl.Guest.Add.loadData = function(e) {
    document.getElementById('addGuestGuestGroupId').value = e.relatedTarget.value;

    if (!GikDefTmpl.Guest.Add.addressBooksLoaded) {
        const hdrs = GikDefTmpl.newXhrHeaders();
        fetch('/controlcenter/addressbooks', {
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
                  const addressbooks = GikDefTmpl.Guest.Add.form.elements.namedItem('addressbook');
                  for (const book of data) {
                      const opt = document.createElement('option');
                      opt.value = book.id;
                      opt.textContent = book.name;
                      addressbooks.appendChild(opt);
                  }
                  GikDefTmpl.Guest.Add.loadContacts(addressbooks.value);
                  GikDefTmpl.Guest.Add.addressBooksLoaded = true;
              })
        .catch(error => {
                   if (error instanceof Response) {
                       error.json().then(json => {
                                             if (json.error) {
                                                 GikDefTmpl.Guest.Add.modal.hide();
                                                 GikDefTmpl.showError(json.error.title, json.error.text);
                                             }
                                         });
                   } else {
                       GikDefTmpl.Guest.Add.modal.hide();
                       GikDefTmpl.showError(error.name, error.message);
                   }
               });
    }
}

GikDefTmpl.Guest.Add.init = function() {
    const agm = document.getElementById('addGuestModal');
    if (agm) {
        GikDefTmpl.Guest.Add.modal = bootstrap.Modal.getOrCreateInstance(agm);
        agm.addEventListener('show.bs.modal', GikDefTmpl.Guest.Add.loadData);

        GikDefTmpl.Guest.Add.form = document.forms['addGuestForm'];
        GikDefTmpl.Guest.Add.form.addEventListener('submit', (e) => { e.preventDefault(); GikDefTmpl.Guest.Add.exec(e); });

        const addressbooks = GikDefTmpl.Guest.Add.form.elements.namedItem('addressbook');
        addressbooks.addEventListener('change', (e) => { GikDefTmpl.Guest.Add.loadContacts(e.target.value); });
    }
}

GikDefTmpl.Guest.Add.init();
