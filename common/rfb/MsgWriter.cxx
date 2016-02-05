/* Copyright (C) 2002-2005 RealVNC Ltd.  All Rights Reserved.
 * Copyright 2009-2014 Pierre Ossman for Cendio AB
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

#include <rdr/OutStream.h>

#include <rfb/fenceTypes.h>
#include <rfb/msgTypes.h>
#include <rfb/ConnParams.h>
#include <rfb/Exception.h>
#include <rfb/MsgWriter.h>

using namespace rfb;

MsgWriter::MsgWriter(bool client_, ConnParams* cp_, rdr::OutStream* os_)
  : cp(cp_), os(os_), client(client_)
{
}

MsgWriter::~MsgWriter()
{
}

void MsgWriter::writeFence(rdr::U32 flags, unsigned len, const char data[])
{
  if (!cp->supportsFence)
    throw Exception("Peer does not support fences");
  if (len > 64)
    throw Exception("Too large fence payload");
  if ((flags & ~fenceFlagsSupported) != 0)
    throw Exception("Unknown fence flags");

  startMsg(client ? msgTypeClientFence : msgTypeServerFence);
  os->pad(3);

  os->writeU32(flags);

  os->writeU8(len);
  os->writeBytes(data, len);

  endMsg();
}

void MsgWriter::writeCutText(const char* str, rdr::U32 len)
{
  startMsg(client ? msgTypeClientCutText : msgTypeServerCutText);
  os->pad(3);
  os->writeU32(len);
  os->writeBytes(str, len);
  endMsg();
}

void MsgWriter::startMsg(int type)
{
  os->writeU8(type);
}

void MsgWriter::endMsg()
{
  os->flush();
}
