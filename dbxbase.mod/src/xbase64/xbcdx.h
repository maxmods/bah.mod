#ifndef cdx_h
#define cdx_h

#include "xbmindex.h"

struct CdxHeader
{
  xbLong rootNode;
  xbLong freeNode;
  xbLong reserved;
  xbShort keyLen;
  union cdxFeatures
  {
    struct Features
    {
      bool unique:1;
      int:2;
      bool hasFor:1;
      bool:1;
      bool cdxHeader:1;
      bool cdxFmt:1;
      bool cdxTagHeader:1;
    } features;
    char feature;
  } features;
  char signature;
  xbLong reserved1[5];
  char reserved2[466];
  xbShort descending;
  xbShort totalExprLen;
  xbShort forExprLen;
  xbShort reserved4;
  xbShort keyExprLen;
  char keyforBuffer[512];
};

struct CdxNode
{
  xbShort attr;
  xbShort keyCount;
  xbLong leftSibling;
  xbLong rightSibling;
}
#ifdef __GNU LesserC__
	__attribute__((packed))
#endif
;

struct CdxInnerNode: public CdxNode
{
  char keys[500];
}
#ifdef __GNU LesserC__
	__attribute__((packed))
#endif
;

struct CdxLeafNode: public CdxNode
{
  xbShort freeSpace;
  xbLong recNumberMask;
  char dupByteCounterMask;
  char tailByteCounterMask;
  char recBitUsing;
  char dupBitUsing;
  char tailBitUsing;
  char byteCount;
  char keys[488];
} 
#ifdef __GNU LesserC__
	__attribute__((packed))
#endif
;

class XBDLLEXPORT xbCdx: public xbMultiIndex
{
  public:
//    xbCdx() {} I don't like to make empty object with no protection 
// to method method call. And I don't see any need of it.
    xbCdx(xbDbf* dbf): xbMultiIndex(dbf) 
    {
      memset(&indexHeader_, 0, sizeof(indexHeader_));
      memset(&tagHeader_, 0, sizeof(tagHeader_));
    }

    virtual ~xbCdx() {CloseIndex();}

    virtual xbShort CreateIndex(const char *filename, const char *expr, 
         xbShort unique, xbShort overwrite);
    virtual xbShort CreateIndex(const char *filename, const char* tagname, 
         const char *expr, xbShort unique, xbShort overwrite);

    virtual xbShort AddTag(const char* tagname, const char *expr, 
         xbShort unique, xbShort overwrite) {return 0;}

    virtual xbLong   GetTotalNodes() {return 0;}
    virtual xbULong  GetCurDbfRec() {return 0;}
    virtual xbShort  CreateKey( xbShort, xbShort ) {return 0;}
    virtual xbShort  GetCurrentKey(char *key) {return 0;}
    virtual xbShort  AddKey( xbLong ) {return 0;}
    virtual xbShort  UniqueIndex() {return 0;}
    virtual xbShort  DeleteKey( xbLong ) {return 0;}
    virtual xbShort  KeyWasChanged() {return 0;}
    virtual xbShort  FindKey( const char * ) {return 0;}
    virtual xbShort  FindKey() {return 0;}
    virtual xbShort  FindKey( xbDouble ) {return 0;}
    virtual xbShort  GetNextKey() {return 0;}
    virtual xbShort  GetLastKey() {return 0;}
    virtual xbShort  GetFirstKey() {return 0;}
    virtual xbShort  GetPrevKey() {return 0;}
    virtual xbShort  ReIndex(void (*statusFunc)(xbLong itemNum, xbLong numItems) = 0) {return 0;}
    virtual xbShort  KeyExists( xbDouble ) {return 0;}
    virtual void     GetExpression(char *buf, int len) {}
#ifdef XBASE_DEBUG
    virtual void     DumpHdrNode( xbShort Option ) {};
    virtual void     DumpNodeRec( xbLong ) {};
    virtual void     DumpNodeChain() {};
    virtual xbShort  CheckIndexIntegrity( xbShort ) {return 0;};
#endif
    
//    static xbString CreateIndexName(const xbString& dbfName);
    virtual const char* GetExtWithDot(bool lower);
    const CdxHeader& GetIndexHeader() {return indexHeader_;}
    const CdxHeader& GetTagHeader() {return tagHeader_;}
  
  protected:
    virtual xbShort GetHeadNode();
    virtual xbUShort GetKeyLen() {return 0;}
    virtual const char* GetKeyExpression() {return "";}
    virtual void FreeNodesMemory() {}
    void ReadTagHeader();
    xbLong GetIndexTagOffset() {return 0;}
    void ReadIndexHeader(xbLong) {}

  private:
    xbCdx(const xbCdx&);
    xbCdx& operator=(const xbCdx&);
    void WriteTagHeader(const char* tagName);
    void WriteTagRoot(const char* tagName);
    void WriteIndexHeader(const char* expr);
    void WriteIndexRoot();
  
  private:
    CdxHeader tagHeader_;
    CdxLeafNode tagRootNode_;
    CdxHeader indexHeader_;
    CdxLeafNode indexRootNode_;
};

#endif
