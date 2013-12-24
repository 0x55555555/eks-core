#ifndef XJSONWRITER_H
#define XJSONWRITER_H

#include "XProperty.h"
#include "Containers/XStringSimple.h"

namespace Eks
{

class EKSCORE_EXPORT JSONWriter
  {
XProperties:
  XROByRefProperty(String, string);
  XProperty(bool, niceFormatting, setNiceFormatting);

public:
  JSONWriter(Eks::AllocatorBase *alloc, const JSONWriter *parent = nullptr);
  ~JSONWriter();

  void clear();

  void tabIn();
  void tabOut();

  void addBlock(const char *data, bool addedElementsToScope);

  void beginArray();
  void beginObject();
  void end();

  void beginObjectElement(const char *key);
  void beginArrayElement();
  void endElement();

  void addValueForElement(const char *data);
  void addKeyValueStandalone(const char *key, const char *value, bool commaFirst);

  void beginNewline();

private:
  struct Scope
    {
    bool hasElements;
    bool hasOpenElement;

    enum Type
      {
      Object,
      Array
      } type;
    };

  void beginScope(Scope::Type type);

  void beginNewlineImpl();

  void addKey(const char *key);

  void appendQuotedEscaped(const char* data);

  class WriteBlock
    {
  public:
    WriteBlock(JSONWriter *writer);

    ~WriteBlock();

  private:
    JSONWriter *_writer;
    WriteBlock *_oldBlock;
    };

  WriteBlock *_activeBlock;

  Eks::Vector<Scope> _stack;

  Eks::String::BaseType::String *_vector;
  Eks::String _indentString;
  };

}

#endif // XJSONWRITER_H
