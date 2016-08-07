/*! @file format-100.h */
#ifndef FORMAT_100_H
#define FORMAT_100_H

#include "emb-pattern.h"

#include "api-start.h"
#ifdef __cplusplus
extern "C" {
#endif

extern EMB_PRIVATE int EMB_CALL read100(EmbPattern* pattern, const char* fileName);
extern EMB_PRIVATE int EMB_CALL write100(EmbPattern* pattern, const char* fileName);

#ifdef __cplusplus
}
#endif /* __cplusplus */
#include "api-stop.h"

#endif /* FORMAT_100_H */

/* kate: bom off; indent-mode cstyle; indent-width 4; replace-trailing-space-save on; */
