#include "function.h"
#include "aggregate/expr/expression.h"
#include "dep/geo/rs_geo.h"
#include <err.h>


/* distance() */

static int geofunc_distance(ExprEval *ctx, RSValue *result,
                            RSValue **argv, size_t argc, QueryError *err) {
  VALIDATE_ARGS("distance", 3, 3, err);
  RSValue *val;
  char *p, *end;

  val = RSValue_Dereference(argv[0]);
  if (!RSValue_IsString(val)) {
    goto error;
  }
  p = (char *)RSValue_StringPtrLen(val, NULL); 
  double geo1[2], geo2[2];
  if (parseGeo(p, &geo1[0], &geo1[1]) != REDISMODULE_OK) {
    goto error;
  }

  for (int i = 0; i < 2; ++i) {
    if (!RSValue_ToNumber(argv[i + 1], &geo2[i])) {
      goto error;
    }
  }

  double distance = geohashGetDistance(geo1[0], geo1[1], geo2[0], geo2[1]);
  distance = round(distance * 100) / 100;
  RSValue_SetNumber(result, distance);
  return EXPR_EVAL_OK;

error:
  RSValue_SetNumber(result, NAN);
  return EXPR_EVAL_OK;
}

void RegisterGeoFunctions() {
  RSFunctionRegistry_RegisterFunction("distance", geofunc_distance, RSValue_String);
}