#include "xbtypes.h"
#include "xbcdx.h"

xbShort xbCdx::CreateIndex(const char* filename, const char *expr, 
     xbShort unique, xbShort overwrite)
{
  return CreateIndex(filename, "NoName", expr, unique, overwrite);
}

xbShort xbCdx::CreateIndex(const char* filename, const char *tagName, 
     const char *expr, xbShort unique, xbShort overwrite)
{
  if (IsOpen()) CloseIndex();
  SetFileName(filename);
  
  indexfp=fopen(GetFileName(), "wb+");
  WriteTagHeader(tagName);
  WriteTagRoot(tagName);
  WriteIndexHeader(expr);
  WriteIndexRoot();
  return 0;
}

const char* xbCdx::GetExtWithDot(bool lower) 
{
  return lower? ".cdx": ".CDX";
}

void xbCdx::WriteTagHeader(const char* tagName)
{
  memset(&tagHeader_, 0, sizeof(tagHeader_));
  tagHeader_.rootNode=0x400;
  tagHeader_.keyLen=strlen(tagName)+1;
  tagHeader_.features.feature=0xe0;
  tagHeader_.signature=1;
  tagHeader_.totalExprLen=1;
  tagHeader_.forExprLen=1;
  tagHeader_.keyExprLen=1;
  fwrite(&tagHeader_, sizeof(tagHeader_), 1, indexfp);
}

void xbCdx::WriteTagRoot(const char* tagName)
{
  memset(&tagRootNode_, 0, sizeof(tagRootNode_));
  tagRootNode_.attr=3;
  tagRootNode_.keyCount=1;
  tagRootNode_.leftSibling=-1;
  tagRootNode_.rightSibling=-1;
  tagRootNode_.freeSpace=476;
  tagRootNode_.recNumberMask=0xffff;
  tagRootNode_.dupByteCounterMask=0xf;
  tagRootNode_.tailByteCounterMask=0xf;
  tagRootNode_.recBitUsing=16;
  tagRootNode_.dupBitUsing=4;
  tagRootNode_.tailBitUsing=4;
  tagRootNode_.byteCount=3;
  xbShort indexHeadOffset=0x600;
  int len=sizeof(indexHeadOffset);
  memcpy(tagRootNode_.keys, &indexHeadOffset, len);
  tagRootNode_.keys[len]=16;
  len=strlen(tagName);
  xbString tag=tagName;
  tag.toUpperCase();
  memcpy(tagRootNode_.keys+sizeof(tagRootNode_.keys)-len, tag.c_str(), len);
  fwrite(&tagRootNode_, sizeof(tagRootNode_), 1, indexfp);
}

void xbCdx::WriteIndexHeader(const char* expr)
{
  memset(&indexHeader_, 0, sizeof(indexHeader_));
  indexHeader_.rootNode=0xa00;
  indexHeader_.keyLen=33;
  indexHeader_.features.feature=0x60;
  indexHeader_.signature=1;
  indexHeader_.totalExprLen=strlen(expr)+1;
  indexHeader_.forExprLen=1;
  indexHeader_.keyExprLen=strlen(expr)+1;
  xbString exprn=expr;
  exprn.toUpperCase();
  memcpy(indexHeader_.keyforBuffer, exprn.c_str(), indexHeader_.keyExprLen);
  fwrite(&indexHeader_, sizeof(indexHeader_), 1, indexfp);
}

void xbCdx::WriteIndexRoot()
{
  memset(&indexRootNode_, 0, sizeof(indexRootNode_));
  indexRootNode_.attr=3;
  indexRootNode_.keyCount=0;
  indexRootNode_.leftSibling=-1;
  indexRootNode_.rightSibling=-1;
  indexRootNode_.freeSpace=488;
  indexRootNode_.recNumberMask=0x0fff;
  indexRootNode_.dupByteCounterMask=0x3f;
  indexRootNode_.tailByteCounterMask=0x3f;
  indexRootNode_.recBitUsing=12;
  indexRootNode_.dupBitUsing=6;
  indexRootNode_.tailBitUsing=6;
  indexRootNode_.byteCount=3;
  fwrite(&indexRootNode_, sizeof(indexRootNode_), 1, indexfp);
}

xbShort xbCdx::GetHeadNode()
{
  ReadTagHeader();
  ReadIndexHeader(GetIndexTagOffset());
  return XB_NO_ERROR;
}

void xbCdx::ReadTagHeader()
{
  _fseek(indexfp, 0, SEEK_SET);
  fread(&tagHeader_, sizeof(tagHeader_), 1, indexfp);
}
