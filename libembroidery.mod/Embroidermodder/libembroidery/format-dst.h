/*! @file format-dst.h */
#ifndef FORMAT_DST_H
#define FORMAT_DST_H

#include "emb-pattern.h"

#include "api-start.h"
#ifdef __cplusplus
extern "C" {
#endif

extern EMB_PRIVATE int EMB_CALL readDst(EmbPattern* pattern, const char* fileName);
extern EMB_PRIVATE int EMB_CALL writeDst(EmbPattern* pattern, const char* fileName);

#ifdef __cplusplus
}
#endif /* __cplusplus */
#include "api-stop.h"

#endif /* FORMAT_DST_H */

/* kate: bom off; indent-mode cstyle; indent-width 4; replace-trailing-space-save on; */
