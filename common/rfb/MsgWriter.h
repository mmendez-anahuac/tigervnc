/* Copyright (C) 2002-2005 RealVNC Ltd.  All Rights Reserved.
 * Copyright 2016 Pierre Ossman for Cendio AB
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
#ifndef __RFB_MSGWRITER_H__
#define __RFB_MSGWRITER_H__

#include <rdr/types.h>

namespace rdr { class OutStream; }

namespace rfb {
  class ConnParams;

  class MsgWriter {
  protected:
    MsgWriter(bool client, ConnParams* cp, rdr::OutStream* os);
    virtual ~MsgWriter();

  public:
    void writeFence(rdr::U32 flags, unsigned len, const char data[]);

    void writeCutText(const char* str, rdr::U32 len);

    void writeClipboardCaps(rdr::U32 caps, const rdr::U32* lengths);
    void writeClipboardRequest(rdr::U32 flags);
    void writeClipboardPeek(rdr::U32 flags);
    void writeClipboardNotify(rdr::U32 flags);
    void writeClipboardProvide(rdr::U32 flags, const size_t* lengths,
                               const rdr::U8* const* data);

  protected:
    void startMsg(int type);
    void endMsg();

    ConnParams* cp;
    rdr::OutStream* os;

  private:
    bool client;
  };

}
#endif
