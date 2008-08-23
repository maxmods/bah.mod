#ifndef xbNode_h
#define xbNode_h

class xbNodeLink
{
  public:
    xbNodeLink(): nextNode_(0) {}
    void AddNode(xbNodeLink* node);
    xbNodeLink* GetNext() {return nextNode_;}

  private:
    xbNodeLink(const xbNodeLink&);
    xbNodeLink& operator=(const xbNodeLink&);

  private:
    xbNodeLink* nextNode_;
};

#endif
