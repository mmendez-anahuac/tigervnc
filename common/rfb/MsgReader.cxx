/* Copyright (C) 2002-2005 RealVNC Ltd.  All Rights Reserved.
 * Copyright 2009-2016 Pierre Ossman for Cendio AB
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

#include <rdr/InStream.h>
#include <rdr/ZlibInStream.h>

#include <rfb/clipboardTypes.h>
#include <rfb/Exception.h>
#include <rfb/LogWriter.h>
#include <rfb/MsgHandler.h>
#include <rfb/MsgReader.h>
#include <rfb/util.h>

using namespace rfb;

static LogWriter vlog("MsgReader");

static IntParameter maxCutText("MaxCutText", "Maximum permitted length of an incoming clipboard update", 256*1024);

MsgReader::MsgReader(MsgHandler* handler, rdr::InStream* is)
  : handler(handler), is(is)
{
}

MsgReader::~MsgReader()
{
}

void MsgReader::readCutText()
{
  is->skip(3);
  rdr::U32 len = is->readU32();

  if (len & 0x80000000) {
    rdr::S32 slen = len;
    slen = -slen;
    readExtendedClipboard(slen);
    return;
  }

  if (len > (size_t)maxCutText) {
    vlog.error("cut text too long (%d bytes) - ignoring",len);
    is->skip(len);
    return;
  }
  CharArray ca(len+1);
  ca.buf[len] = 0;
  is->readBytes(ca.buf, len);
  handler->cutText(ca.buf, len);
}

void MsgReader::readExtendedClipboard(rdr::S32 len)
{
  rdr::U32 flags;
  rdr::U32 action;

  if (len < 4)
    throw Exception("Invalid extended clipboard message");
  if (len > maxCutText) {
    vlog.error("Extended clipboard message too long (%d bytes) - ignoring", len);
    is->skip(len);
    return;
  }

  flags = is->readU32();
  action = flags & clipboardActionMask;

  if (action & clipboardCaps) {
    int i;
    size_t num;
    rdr::U32 lengths[16];

    num = 0;
    for (i = 0;i < 16;i++) {
      if (flags & (1 << i))
        num++;
    }

    if (len < (rdr::S32)(4 + 4*num))
      throw Exception("Invalid extended clipboard message");

    num = 0;
    for (i = 0;i < 16;i++) {
      if (flags & (1 << i))
        lengths[num++] = is->readU32();
    }

    handler->clipboardCaps(flags, lengths);
  } else if (action == clipboardProvide) {
    rdr::ZlibInStream zis;

    int i;
    size_t num;
    size_t lengths[16];
    rdr::U8* buffers[16];

    zis.setUnderlying(is, len - 4);

    num = 0;
    for (i = 0;i < 16;i++) {
      if (!(flags & 1 << i))
        continue;

      lengths[num] = zis.readU32();
      if (lengths[num] > (size_t)maxCutText) {
        vlog.error("Extended clipboard data too long (%d bytes) - ignoring",
	           (unsigned)lengths[num]);
	zis.skip(lengths[num]);
	flags &= ~(1 << i);
	continue;
      }

      buffers[num] = new rdr::U8[lengths[num]];
      zis.readBytes(buffers[num], lengths[num]);
      num++;
    }

    zis.removeUnderlying();

    handler->clipboardProvide(flags, lengths, buffers);

    num = 0;
    for (i = 0;i < 16;i++) {
      if (!(flags & 1 << i))
        continue;
      delete [] buffers[num++];
    }
  } else {
    switch (action) {
    case clipboardRequest:
      handler->clipboardRequest(flags);
      break;
    case clipboardPeek:
      handler->clipboardPeek(flags);
      break;
    case clipboardNotify:
      handler->clipboardNotify(flags);
      break;
    default:
      throw Exception("Invalid extended clipboard action");
    }
  }
}

void MsgReader::readFence()
{
  rdr::U32 flags;
  rdr::U8 len;
  char data[64];

  is->skip(3);

  flags = is->readU32();

  len = is->readU8();
  if (len > sizeof(data)) {
    vlog.error("Ignoring fence with too large payload");
    is->skip(len);
    return;
  }

  is->readBytes(data, len);

  handler->fence(flags, len, data);
}
