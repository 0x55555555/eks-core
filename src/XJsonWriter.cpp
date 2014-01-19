#include "Utilities/XJsonWriter.h"

namespace Eks
{

JSONWriter::JSONWriter(AllocatorBase *alloc, const JSONWriter *parent)
  : _string(alloc),
    _niceFormatting(false),
    _activeBlock(nullptr),
    _stack(alloc),
    _vector(&_string),
    _indentString(alloc)
  {
  if(parent)
    {
    _indentString = parent->_indentString;
    _niceFormatting = parent->niceFormatting();
    }
  }

JSONWriter::~JSONWriter()
  {
  xAssert(_stack.size() == 0);
  }

void JSONWriter::clear()
  {
  _string.clear();
  }

void JSONWriter::tabIn()
  {
  _indentString.append("\t");
  }

void JSONWriter::tabOut()
  {
  _indentString.resize(_indentString.size() - 1, '\t');
  }

void JSONWriter::addBlock(const char *data, bool addedElementsToScope)
  {
  xAssert(data);
  WriteBlock b(this);
  beginNewlineImpl();

  _vector->append(data, strlen(data));

  if (addedElementsToScope)
    {
    xAssert(_stack.size());
    auto &obj = _stack.back();

    obj.hasElements = addedElementsToScope;
    }
  }

void JSONWriter::beginArray()
  {
  beginScope(Scope::Array);
  }

void JSONWriter::beginObject()
  {
  beginScope(Scope::Object);
  }

void JSONWriter::beginScope(JSONWriter::Scope::Type type)
  {
  WriteBlock b(this);

  if(type == Scope::Object)
    {
    _vector->pushBack('{');
    }
  else
    {
    _vector->pushBack('[');
    }

  tabIn();

  auto &obj = _stack.createBack();
  obj.hasElements = false;
  obj.hasOpenElement = false;
  obj.type = type;
  }

void JSONWriter::end()
  {
  WriteBlock b(this);

  xAssert(_stack.size());
  auto &obj = _stack.back();

  xAssert(!obj.hasOpenElement);
  tabOut();

  if(obj.hasElements)
    {
    beginNewlineImpl();
    }

  if(_stack.back().type == Scope::Object)
    {
    _vector->pushBack('}');
    }
  else
    {
    _vector->pushBack(']');
    }

  _stack.popBack();
  }

void JSONWriter::beginObjectElement(const char *key)
  {
  WriteBlock b(this);

  xAssert(_stack.size());

  auto &back = _stack.back();
  xAssert(back.type == Scope::Object);
  xAssert(back.hasOpenElement == false);

  if(_stack.back().hasElements)
    {
    _vector->pushBack(',');
    }

  addKey(key);
  back.hasOpenElement = true;
  }

void JSONWriter::beginArrayElement()
  {
  xAssert(_stack.size())
      xAssert(_stack.back().type == Scope::Array);
  xAssert(_stack.back().hasOpenElement == false);

  WriteBlock b(this);

  if(_stack.back().hasElements)
    {
    _vector->pushBack(',');
    }

  beginNewlineImpl();
  _stack.back().hasOpenElement = true;
  }

void JSONWriter::endElement()
  {
  xAssert(_stack.size());

  xAssert(_stack.back().hasOpenElement == true);
  _stack.back().hasElements = true;
  _stack.back().hasOpenElement = false;
  }

void JSONWriter::addValueForElement(const char *data)
  {
  xAssert(data);
  WriteBlock b(this);

  appendQuotedEscaped(data);

  xAssert(_stack.size());
  _stack.back().hasElements = true;
  }

void JSONWriter::addKeyValueStandalone(const char *key, const char *value, bool commaFirst)
  {
  xAssert(key);
  xAssert(value);
  WriteBlock b(this);

  if(commaFirst)
    {
    _vector->pushBack(',');
    }

  addKey(key);

  appendQuotedEscaped(value);
  }

void JSONWriter::beginNewline()
  {
  WriteBlock b(this);
  beginNewlineImpl();
  }

void JSONWriter::beginNewlineImpl()
  {
  xAssert(_activeBlock);

  if (_niceFormatting && _vector->length())
    {
    _vector->pushBack('\n');
    _vector->append(_indentString.data(), _indentString.size());
    }
  }

void JSONWriter::addKey(const char *key)
  {
  WriteBlock b(this);

  beginNewlineImpl();

  appendQuotedEscaped(key);

  if(_niceFormatting)
    {
    _vector->append(": ", 2);
    }
  else
    {
    _vector->pushBack(':');
    }
  }

void JSONWriter::appendQuotedEscaped(const char *data)
  {
  xAssert(_activeBlock);

  _vector->pushBack('"');


  for(const char* p = data; *p; ++p)
    {
    if(*p == '"')
      {
      _vector->pushBack('\\');
      }

    _vector->pushBack(*p);
    }

  _vector->pushBack('"');
  }

JSONWriter::WriteBlock::WriteBlock(JSONWriter *writer) : _writer(writer)
  {
  _oldBlock = _writer->_activeBlock;
  _writer->_activeBlock = this;

  if (!_oldBlock && _writer->_vector->size())
    {
    _writer->_vector->popBack();
    }
  }

JSONWriter::WriteBlock::~WriteBlock()
  {
  _writer->_activeBlock = _oldBlock;

  if (!_writer->_activeBlock)
    {
    _writer->_vector->pushBack('\0');
    }
  }

}
