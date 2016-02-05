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
#ifndef __RFB_MSGHANDLER_H__
#define __RFB_MSGHANDLER_H__

namespace rfb {

  class MsgHandler {
  public:
    virtual ~MsgHandler() {}

    // The following methods are called as corresponding messages are
    // read. A derived class should override these methods as desired.

    virtual void fence(rdr::U32 flags, unsigned len, const char data[]) = 0;

    virtual void cutText(const char* str, rdr::U32 len) = 0;

    virtual void clipboardCaps(rdr::U32 flags, const rdr::U32* lengths) = 0;
    virtual void clipboardRequest(rdr::U32 flags) = 0;
    virtual void clipboardPeek(rdr::U32 flags) = 0;
    virtual void clipboardNotify(rdr::U32 flags) = 0;
    virtual void clipboardProvide(rdr::U32 flags,
                                  const size_t* lengths,
                                  const rdr::U8* const* data) = 0;
  };

}
#endif
