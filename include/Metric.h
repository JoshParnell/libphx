#ifndef PHX_Metric
#define PHX_Metric

#include "Common.h"

/* --- API NOTES ---------------------------------------------------------------
 *
 *   The Metric API is a simple set of accumulators that engine functions may
 *   call to provide information relevent to performance profiling and
 *   debugging. Metrics are reset each frame in Engine_Update, hence provide
 *   only instantaneous information.
 *
 *   Metrics are quantities that only code internal to the engine can change
 *   (e.g., a Lua script cannot draw a polygon without passing through
 *   Draw_*, Mesh_Draw*, etc). Hence, all functions that modify metrics are
 *   private.
 *
 *   Metric_Reset is called automatically in Engine_Update, so that metrics
 *   are per-frame.
 *
 *     DrawCalls  : # gl_Draw* calls. glBegin/glEnd does not count.
 *     Immediate  : # glBegin/glEnd pairs.
 *     PolysDrawn : # polys drawn. gl_Begin(GL_QUADS) et al. should report the
 *                  number of *polygons*, not triangles!
 *
 *     TrisDrawn  : # tris drawn. Non-tri draw calls should report
 *                  #verts - 2, e.g. number of tris required for convex
 *                  decomposition.
 *
 *     VertsDrawn : # verts *referenced* in draw calls. Buffered draws could
 *                  report only the size of the vertex buffer! Immediate
 *                  mode should report # calls to glVertex*.
 *
 *     Flush      : An *upper bound* on pipeline flushes. Each call that can
 *                  cause a flush should increment this metric.
 *
 *     FBOSwap    : # glBindFramebuffer calls. Each call should be reported.
 *                  In the future, we will introduce a new metric to report #
 *                  of validation-inducing bind calls, but this will only be
 *                  relevant once fbo caching is implemented.
 *
 * -------------------------------------------------------------------------- */

const Metric Metric_None       = 0x0;
const Metric Metric_DrawCalls  = 0x1;
const Metric Metric_Immediate  = 0x2;
const Metric Metric_PolysDrawn = 0x3;
const Metric Metric_TrisDrawn  = 0x4;
const Metric Metric_VertsDrawn = 0x5;
const Metric Metric_Flush      = 0x6;
const Metric Metric_FBOSwap    = 0x7;
const Metric Metric_SIZE       = 0x7;

PHX_API int32  Metric_Get         (Metric);
PHX_API cstr   Metric_GetName     (Metric);

/* --- Private API ---------------------------------------------------------- */

PRIVATE void   Metric_Inc         (Metric);
PRIVATE void   Metric_Mod         (Metric, int32);
PRIVATE void   Metric_Reset       ();

PRIVATE void   Metric_AddDraw     (int32 polys, int32 tris, int32 verts);
PRIVATE void   Metric_AddDrawImm  (int32 polys, int32 tris, int32 verts);

#endif
