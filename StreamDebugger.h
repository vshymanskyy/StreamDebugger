/**
 * @file       StreamDebugger.h
 * @author     Volodymyr Shymanskyy
 * @license    This project is released under the MIT License (MIT)
 * @copyright  Copyright (c) 2016 Volodymyr Shymanskyy
 * @date       Nov 2016
 */

#ifndef StreamDebugger_h
#define StreamDebugger_h

#include <Arduino.h>

class StreamDebugger
  : public Stream
{
  public:
    StreamDebugger() {}
    StreamDebugger(Stream& data, Stream& dump) {
      setDataStream(&data);
      setDumpStream(&dump);
    }
    
    virtual ~StreamDebugger() {}

    virtual size_t write(uint8_t ch) {
      if (_dump)
        _dump->write(ch);
      return _data ? _data->write(ch) : 0;
    }
    virtual int read() {
      if (!_data)
        return -1;
      int ch = _data->read();
      if (ch != -1 && _dump) {
        _dump->write(ch);
      }
      return ch;
    }
    virtual int available() { return _data ? _data->available() : 0; }
    virtual int peek() { return _data ? _data->peek() : -1; }
    virtual void flush() {
      if (_data)
        _data->flush();
    }

    void directAccess() {
      int r;
      while (true) {
        if (_data && _data->available() && (r = _data->read()) != -1 && _dump)
          _dump->write(r);
        if (_dump && _dump->available() && (r = _dump->read()) != -1 && _data)
          _data->write(r);
        delay(0);
      }
    }

    void directAccessNonBlocking() {
      int r;
      while (_data && _data->available() && (r = _data->read()) != -1)
        if (_dump)
          _dump->write(r);
      while (_dump && _dump->available() && (r = _dump->read()) != -1)
        if (_data)
          _data->write(r);
    }

    void setDataStream(Stream* data) {
      _data = data;
    }

    void setDumpStream(Stream* dump) {
      _dump = dump;
    }

  private:
    Stream* _data = nullptr;
    Stream* _dump = nullptr;
};

#endif
