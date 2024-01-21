#include "tumble/errors.h"

const char *tmb_strerror(tmb_err err)
{
    switch (err) {
    case TMB_OK:              return "ok";
    case TMB_ERR_IO:          return "I/O error";
    case TMB_ERR_OOM:         return "out of memory";
    case TMB_ERR_PARSE:       return "malformed scene";
    case TMB_ERR_BAD_MAGIC:   return "not a Tumble binary scene (bad signature)";
    case TMB_ERR_BAD_VERSION: return "unsupported scene version";
    case TMB_ERR_TRUNCATED:   return "input ended unexpectedly";
    case TMB_ERR_RANGE:       return "count or size exceeds limit";
    case TMB_ERR_INDEX:       return "reference to a missing body or shape";
    case TMB_ERR_DEGENERATE:  return "degenerate shape";
    case TMB_ERR_EMPTY:       return "scene is empty";
    case TMB_ERR_UNSUPPORTED: return "unsupported feature";
    case TMB_ERR__COUNT:      break;
    }
    return "unknown error";
}
