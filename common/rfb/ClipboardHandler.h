/* Copyright 2016 Pierre Ossman for Cendio AB
 * 
 * This is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 * 
 * This software is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with this software; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307,
 * USA.
 */
//
// ClipboardHandler - abstract interface for bi-directional clipboard
// handling. Roles and naming is based on if the object is upstream or
// downstream of the network transport. All data is UTF-8 text with
// only LF used as line termination.
//

#ifndef __RFB_CLIPBOARDHANDLER_H__
#define __RFB_CLIPBOARDHANDLER_H__

namespace rfb {

  class ClipboardHandler {
  public:
    virtual ~ClipboardHandler() {}

    // remoteClipboardAvailable()/remoteClipboardUnavailable() is
    // called to indicate the status of the clipboard on the remote
    // peer. Call remoteClipboardRequest() on the relevant object to
    // access the actual data. remoteClipboardAvailable() might be
    // called multiple times without an intervening call to
    // remoteClipboardUnavailable().
    virtual void remoteClipboardAvailable() = 0;
    virtual void remoteClipboardUnavailable() = 0;

    // remoteClipboardData() is called as a result of a previous call
    // to remoteClipboardRequest() on the relevant object. Note that
    // this function might never be called if no data was available
    // when the request was handled.
    virtual void remoteClipboardData(const char* data) = 0;

    // remoteClipboardRequest() will result in a request to the remote
    // peer to transfer its clipboard data. A call do
    // remoteClipboardData() will eventually be made if the data is
    // available.
    virtual void remoteClipboardRequest() = 0;

    // Identical behaviour to those above, but regarding the local
    // clipboard data rather than that of a network peer.
    virtual void localClipboardAvailable() = 0;
    virtual void localClipboardUnavailable() = 0;
    virtual void localClipboardData(const char* data) = 0;
    virtual void localClipboardRequest() = 0;
  };

}
#endif
