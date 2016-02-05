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
#include <stdio.h>
#include <rdr/InStream.h>
#include <rfb/msgTypes.h>
#include <rfb/Exception.h>
#include <rfb/util.h>
#include <rfb/SMsgHandler.h>
#include <rfb/SMsgReader.h>
#include <rfb/Configuration.h>
#include <rfb/LogWriter.h>

using namespace rfb;

static LogWriter vlog("SMsgReader");

SMsgReader::SMsgReader(SMsgHandler* handler_, rdr::InStream* is_)
  : MsgReader(handler_, is_), handler(handler_)
{
}

SMsgReader::~SMsgReader()
{
}

void SMsgReader::readClientInit()
{
  bool shared = is->readU8();
  handler->clientInit(shared);
}

void SMsgReader::readMsg()
{
  int msgType = is->readU8();
  switch (msgType) {
  case msgTypeSetPixelFormat:
    readSetPixelFormat();
    break;
  case msgTypeSetEncodings:
    readSetEncodings();
    break;
  case msgTypeSetDesktopSize:
    readSetDesktopSize();
    break;
  case msgTypeFramebufferUpdateRequest:
    readFramebufferUpdateRequest();
    break;
  case msgTypeEnableContinuousUpdates:
    readEnableContinuousUpdates();
    break;
  case msgTypeClientFence:
    readFence();
    break;
  case msgTypeKeyEvent:
    readKeyEvent();
    break;
  case msgTypePointerEvent:
    readPointerEvent();
    break;
  case msgTypeClientCutText:
    readCutText();
    break;
  default:
    fprintf(stderr, "unknown message type %d\n", msgType);
    throw Exception("unknown message type");
  }
}

void SMsgReader::readSetPixelFormat()
{
  is->skip(3);
  PixelFormat pf;
  pf.read(is);
  handler->setPixelFormat(pf);
}

void SMsgReader::readSetEncodings()
{
  is->skip(1);
  int nEncodings = is->readU16();
  rdr::S32Array encodings(nEncodings);
  for (int i = 0; i < nEncodings; i++)
    encodings.buf[i] = is->readU32();
  handler->setEncodings(nEncodings, encodings.buf);
}

void SMsgReader::readSetDesktopSize()
{
  int width, height;
  int screens, i;
  rdr::U32 id, flags;
  int sx, sy, sw, sh;
  ScreenSet layout;

  is->skip(1);

  width = is->readU16();
  height = is->readU16();

  screens = is->readU8();
  is->skip(1);

  for (i = 0;i < screens;i++) {
    id = is->readU32();
    sx = is->readU16();
    sy = is->readU16();
    sw = is->readU16();
    sh = is->readU16();
    flags = is->readU32();

    layout.add_screen(Screen(id, sx, sy, sw, sh, flags));
  }

  handler->setDesktopSize(width, height, layout);
}

void SMsgReader::readFramebufferUpdateRequest()
{
  bool inc = is->readU8();
  int x = is->readU16();
  int y = is->readU16();
  int w = is->readU16();
  int h = is->readU16();
  handler->framebufferUpdateRequest(Rect(x, y, x+w, y+h), inc);
}

void SMsgReader::readEnableContinuousUpdates()
{
  bool enable;
  int x, y, w, h;

  enable = is->readU8();

  x = is->readU16();
  y = is->readU16();
  w = is->readU16();
  h = is->readU16();

  handler->enableContinuousUpdates(enable, x, y, w, h);
}

void SMsgReader::readKeyEvent()
{
  bool down = is->readU8();
  is->skip(2);
  rdr::U32 key = is->readU32();
  handler->keyEvent(key, down);
}

void SMsgReader::readPointerEvent()
{
  int mask = is->readU8();
  int x = is->readU16();
  int y = is->readU16();
  handler->pointerEvent(Point(x, y), mask);
}
