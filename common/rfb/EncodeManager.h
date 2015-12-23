/* Copyright (C) 2000-2003 Constantin Kaplinsky.  All Rights Reserved.
 * Copyright (C) 2011 D. R. Commander.  All Rights Reserved.
 * Copyright 2014 Pierre Ossman for Cendio AB
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
#ifndef __RFB_ENCODEMANAGER_H__
#define __RFB_ENCODEMANAGER_H__

#include <list>
#include <vector>

#include <os/Thread.h>

#include <rdr/types.h>
#include <rfb/PixelBuffer.h>

namespace os {
  class Condition;
  class Mutex;
}

namespace rdr {
  class Exception;
  class MemOutStream;
}

namespace rfb {
  class SConnection;
  class ConnParams;
  class Encoder;
  class UpdateInfo;
  class Palette;
  class PixelBuffer;
  class RenderedCursor;
  class Region;
  class Rect;

  class EncodeManager {
  public:
    EncodeManager(SConnection* conn);
    ~EncodeManager();

    void logStats();

    // Hack to let ConnParams calculate the client's preferred encoding
    static bool supported(int encoding);

    void writeUpdate(const UpdateInfo& ui, const PixelBuffer* pb,
                     const RenderedCursor* renderedCursor);

  protected:
    void prepareEncoders();

    int computeNumRects(const Region& changed);

    Encoder *startRect(const Rect& rect, int type);
    void endRect();

    Encoder* getEncoder(int type);

    void writeCopyRects(const UpdateInfo& ui);
    void writeSolidRects(Region *changed, const PixelBuffer* pb);
    void findSolidRect(const Rect& rect, Region *changed, const PixelBuffer* pb);
    void writeRects(const Region& changed, const PixelBuffer* pb);

    void queueSubRect(const Rect& rect, const PixelBuffer *pb);

    bool checkSolidTile(const Rect& r, const rdr::U8* colourValue,
                        const PixelBuffer *pb);
    void extendSolidAreaByBlock(const Rect& r, const rdr::U8* colourValue,
                                const PixelBuffer *pb, Rect* er);
    void extendSolidAreaByPixel(const Rect& r, const Rect& sr,
                                const rdr::U8* colourValue,
                                const PixelBuffer *pb, Rect* er);
  private:
    // Preprocessor generated, optimised methods
    inline bool checkSolidTile(const Rect& r, rdr::U8 colourValue,
                               const PixelBuffer *pb);
    inline bool checkSolidTile(const Rect& r, rdr::U16 colourValue,
                               const PixelBuffer *pb);
    inline bool checkSolidTile(const Rect& r, rdr::U32 colourValue,
                               const PixelBuffer *pb);

  private:
    void flush();

  protected:
    SConnection *conn;

    std::vector<Encoder*> encoders;
    std::vector<int> activeEncoders;

    struct EncoderStats {
      unsigned rects;
      unsigned long long bytes;
      unsigned long long pixels;
      unsigned long long equivalent;
    };
    typedef std::vector< std::vector<struct EncoderStats> > StatsVector;

    unsigned updates;
    EncoderStats copyStats;
    StatsVector stats;
    int activeType;
    int beforeLength;

    class RectEntry {
    public:
      Rect rect;
      const PixelBuffer* pb;
      const ConnParams* cp;
    };

    class PreparedEntry {
    public:
      ~PreparedEntry();
    public:
      const PixelBuffer* pb;
      const ConnParams* cp;
      int type;
      const Palette* palette;
    };

    class OutputEntry {
    public:
      ~OutputEntry();
    public:
      Rect rect;
      int type;
      const rdr::MemOutStream* buffer;
    };

    std::list<RectEntry*> workQueue;
    size_t rectCount;

    std::vector< std::list<PreparedEntry*> > encoderQueue;

    std::list<OutputEntry*> outputQueue;

    os::Mutex* queueMutex;
    os::Condition* producerCond;
    os::Condition* consumerCond;

  protected:
    class EncodeThread : public os::Thread {
    public:
      EncodeThread(EncodeManager* manager);
      ~EncodeThread();

      void stop();

    protected:
      void worker();

      PreparedEntry* prepareRect(const Rect& rect,
                                 const PixelBuffer* pb,
                                 const ConnParams& cp);

      OutputEntry* encodeRect(const PixelBuffer* pb,
                              const ConnParams& cp,
                              int type,
                              const Palette& palette);

      const PixelBuffer* preparePixelBuffer(const Rect& rect,
                                            const PixelBuffer* pb,
                                            const ConnParams& cp,
                                            bool convert);

      bool analyseRect(const PixelBuffer* pb, int* rleRuns,
                       Palette* palette, int maxColours);

    private:
      // Preprocessor generated, optimised methods
      inline bool analyseRect(int width, int height,
                              const rdr::U8* buffer, int stride,
                              int* rleRuns, Palette* palette,
                              int maxColours);
      inline bool analyseRect(int width, int height,
                              const rdr::U16* buffer, int stride,
                              int* rleRuns, Palette* palette,
                              int maxColours);
      inline bool analyseRect(int width, int height,
                              const rdr::U32* buffer, int stride,
                              int* rleRuns, Palette* palette,
                              int maxColours);

    private:
      EncodeManager* manager;

      bool stopRequested;
    };

    std::list<EncodeThread*> threads;
  };
}

#endif
