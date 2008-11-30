/*
  Copyright (C) 2003, 2005 GraphicsMagick Group
 
  This program is covered by multiple licenses, which are described in
  Copyright.txt. You should have received a copy of Copyright.txt with this
  package; otherwise see http://www.graphicsmagick.org/www/Copyright.html.

  Interfaces to deal with marshalling quantums to and from a bit-stream.
  Written by Bob Friesenhahn, September 2003
 
*/
#ifndef _MAGICK_BIT_STREAM_H
#define _MAGICK_BIT_STREAM_H

#if defined(__cplusplus) || defined(c_plusplus)
extern "C" {
#endif

  static const unsigned int BitAndMasks[32] =
    {
      /*
        Same as (~(~0 << retrieve_bits))
      */
      0x00000000U, 0x00000001U, 0x00000003U, 0x00000007U, 0x0000000fU,
      0x0000001fU, 0x0000003fU, 0x0000007fU, 0x000000ffU, 0x000001ffU,
      0x000003ffU, 0x000007ffU, 0x00000fffU, 0x00001fffU, 0x00003fffU,
      0x00007fffU, 0x0000ffffU, 0x0001ffffU, 0x0003ffffU, 0x0007ffffU,
      0x000fffffU, 0x001fffffU, 0x003fffffU, 0x007fffffU, 0x00ffffffU,
      0x01ffffffU, 0x03ffffffU, 0x07ffffffU, 0x0fffffffU, 0x1fffffffU,
      0x3fffffffU, 0x7fffffffU
    };

  /*
    Bit stream reader "handle"
  */
  typedef struct _BitStreamReadHandle
  {
    const unsigned char  *bytes;
    unsigned int          bits_remaining;
  } BitStreamReadHandle;

  /*
    Initialize Bit Stream for reading
  */
  static inline void BitStreamInitializeRead(BitStreamReadHandle *bit_stream,
                                             const unsigned char *bytes)
  {
    bit_stream->bytes          = bytes;
    bit_stream->bits_remaining = 8;
  }

  /*
    Return the requested number of bits from the current position in a
    bit stream. Stream is read in most-significant bit/byte "big endian"
    order.

    bit_stream      - already initialized bit stream.
    requested_bits  - number of bits to read
  */
  static inline unsigned int BitStreamMSBRead(BitStreamReadHandle *bit_stream,
                                              const unsigned int requested_bits)
  {
    register unsigned int
      remaining_quantum_bits,
      quantum;

    remaining_quantum_bits = requested_bits;
    quantum = 0;

    while (remaining_quantum_bits != 0)
      {
        register unsigned int
          octet_bits;

        octet_bits = remaining_quantum_bits;
        if (octet_bits > bit_stream->bits_remaining)
          octet_bits = bit_stream->bits_remaining;

        remaining_quantum_bits -= octet_bits;
        bit_stream->bits_remaining -= octet_bits;

        quantum = (quantum << octet_bits) |
          ((*bit_stream->bytes >> (bit_stream->bits_remaining))
           & BitAndMasks[octet_bits]);

        if (bit_stream->bits_remaining == 0)
          {
            bit_stream->bytes++;
            bit_stream->bits_remaining=8;
          }
      }
    return quantum;
  }

  /*
    Bit stream writer "handle"
  */
  typedef struct _BitStreamWriteHandle
  {
    unsigned char  *bytes;
    unsigned int    bits_remaining;
  } BitStreamWriteHandle;

  /*
    Initialize Bit Stream for writing

    bit_stream  - bit stream to initialize.
    bytes       - byte array to write bits to
  */
  static inline void BitStreamInitializeWrite(BitStreamWriteHandle *bit_stream,
                                              unsigned char *bytes)
  {
    bit_stream->bytes          = bytes;
    bit_stream->bits_remaining = 8;
  }

  /*
    Write quantum using the specified number of bits at the current
    position in the bit stream. Stream is written in most-significant
    bit/byte "big endian" order.

    bit_stream      - already initialized bit stream.
    requested_bits  - number of bits to write to stream.
    quantum         - value to write.
  */
  static inline void BitStreamMSBWrite(BitStreamWriteHandle *bit_stream,
                                       const unsigned int requested_bits,
                                       const unsigned int quantum)
  {
    register unsigned int
      remaining_quantum_bits = requested_bits;

    while (remaining_quantum_bits != 0)
      {
        register unsigned int
          octet_bits;

        octet_bits = remaining_quantum_bits;
        if (octet_bits > bit_stream->bits_remaining)
          octet_bits = bit_stream->bits_remaining;

        remaining_quantum_bits -= octet_bits;

        if (bit_stream->bits_remaining == 8)
          *bit_stream->bytes = 0;

        bit_stream->bits_remaining -= octet_bits;

        *bit_stream->bytes |=
          (((quantum >> (remaining_quantum_bits)) &
            BitAndMasks[octet_bits]) << (bit_stream->bits_remaining));

        if (bit_stream->bits_remaining == 0)
          {
            bit_stream->bytes++;
            bit_stream->bits_remaining=8;
          }
      }
  }

  /*
    Word reading function.

    read_func_state  - state to pass to word reading function.
  */
  typedef unsigned long (*WordStreamReadFunc) (void *read_func_state);
  
  /*
    Word stream word reader "handle"
  */
  typedef struct _WordStreamReadHandle
  {
    magick_uint32_t      word;
    unsigned int         bits_remaining;
    WordStreamReadFunc   read_func;
    void                *read_func_state;
  } WordStreamReadHandle;
  
  /*
    Initialize Word Stream for reading

    word_stream     - stream to initialize.
    read_func       - function to retrieve the next word.
    read_func_state - state to pass to read_func.
  */
  static inline void WordStreamInitializeRead(WordStreamReadHandle *word_stream,
                                              WordStreamReadFunc read_func,
                                              void *read_func_state)
  {
    word_stream->word            = 0;
    word_stream->bits_remaining  = 0;
    word_stream->read_func       = read_func;
    word_stream->read_func_state = read_func_state;
  }
  
  /*
    Return the requested number of bits from the current position in a
    32-bit word stream. Stream is read starting with the least significant
    bits of the word.

    word_stream     - an initialized word reader stream.
    requested_bits  - number of bits to retrieve from the stream.
  */
  static inline unsigned int WordStreamLSBRead(WordStreamReadHandle *word_stream,
                                               const unsigned int requested_bits)
  {
    register unsigned int
      remaining_quantum_bits,
      quantum;
    
    remaining_quantum_bits = requested_bits;
    quantum = 0;
    
    while (remaining_quantum_bits != 0)
      {
        register unsigned int
          word_bits;
        
        if (word_stream->bits_remaining == 0)
          {
            word_stream->word=word_stream->read_func(word_stream->read_func_state);
            word_stream->bits_remaining=32;
          }
        
        word_bits = remaining_quantum_bits;
        if (word_bits > word_stream->bits_remaining)
          word_bits = word_stream->bits_remaining;
        
        quantum |= (((word_stream->word >> (32-word_stream->bits_remaining))
                     & BitAndMasks[word_bits]) << (requested_bits-remaining_quantum_bits));
        
        remaining_quantum_bits -= word_bits;
        word_stream->bits_remaining -= word_bits;
      }
    return quantum;
  }

  /*
    Word writing function.

    write_func_state  - state to pass to word writing function.
    value             - value to write
    returns number of bytes written.
  */
  typedef size_t (*WordStreamWriteFunc) (void *write_func_state,
                                         const unsigned long value);

  /*
    Word stream writer "handle"
  */
  typedef struct _WordStreamWriteHandle
  {
    magick_uint32_t      word;
    unsigned int         bits_remaining;
    WordStreamWriteFunc  write_func;
    void                *write_func_state;
  } WordStreamWriteHandle;

  /*
    Initialize Word Stream for writing

    word_stream      - stream to initialize.
    write_func_state - state to pass to write_func.
    write_func        - function to retrieve the next word.
  */
  static inline void WordStreamInitializeWrite(WordStreamWriteHandle *word_stream,
                                               WordStreamWriteFunc write_func,
                                               void *write_func_state)
  {
    word_stream->word             = 0U;
    word_stream->bits_remaining   = 32U;
    word_stream->write_func       = write_func;
    word_stream->write_func_state = write_func_state;
  }

  /*
    Write quantum using the specified number of bits at the current
    position in a 32-bit word stream. Samples are output to words
    starting at the least significant bits of the word.

    Note that since a callback function is used to output the words,
    the remaining bits in the last word need to be flushed out by
    invoking WordStreamLSBWriteFlush().

    word_stream     - already initialized word stream.
    requested_bits  - number of bits to write to stream.
    quantum         - value to write.
  */
  static inline void WordStreamLSBWrite(WordStreamWriteHandle *word_stream,
                                        const unsigned int requested_bits,
                                        const unsigned int quantum)
  {
    register unsigned int
      remaining_quantum_bits = requested_bits;

    while (remaining_quantum_bits > 0U)
      {
        register unsigned int
          word_bits;

        word_bits = remaining_quantum_bits;
        if (word_bits > word_stream->bits_remaining)
          word_bits = word_stream->bits_remaining;

        word_stream->word |=
          (((quantum >> (requested_bits-remaining_quantum_bits)) &
            BitAndMasks[word_bits]) << (32-word_stream->bits_remaining));

        remaining_quantum_bits -= word_bits;
        word_stream->bits_remaining -= word_bits;

        if (word_stream->bits_remaining == 0U)
          {
            (void) word_stream->write_func(word_stream->write_func_state,
                                           word_stream->word);
            word_stream->word=0U;
            word_stream->bits_remaining=32U;
          }
      }
  }

  /*
    Write the current output word, regardless of completion.  Unset bits
    are set to zero.  Should be used to ensure that last word in word
    stream is written to the output.  May also be used to apply
    word-level padding at the end of an image row.

    word_stream     - already initialized word stream.
  */  
  static inline void WordStreamLSBWriteFlush(WordStreamWriteHandle *word_stream)
  {
    if (word_stream->bits_remaining != 32U)
      WordStreamLSBWrite(word_stream,word_stream->bits_remaining,0U);
  }

#if defined(__cplusplus) || defined(c_plusplus)
}
#endif

#endif /* _MAGICK_BIT_STREAM_H */
