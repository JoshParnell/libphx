#include "PhxMemory.h"
#include "Metric.h"

static int32 valueCurr[Metric_SIZE + 1] = { 0 };

int32 Metric_Get (Metric self) {
  return valueCurr[self];
}

cstr Metric_GetName (Metric self) {
  switch (self) {
    case Metric_DrawCalls:  return "Draw Calls";
    case Metric_Immediate:  return "Draw Calls (Immediate)";
    case Metric_PolysDrawn: return "Polys";
    case Metric_TrisDrawn:  return "Tris";
    case Metric_VertsDrawn: return "Vertices";
    case Metric_Flush:      return "Pipeline Flushes";
    case Metric_FBOSwap:    return "Framebuffer Swaps";
  }
  return 0;
}

void Metric_AddDraw (int32 polys, int32 tris, int32 verts) {
  valueCurr[Metric_DrawCalls]  += 1;
  valueCurr[Metric_PolysDrawn] += polys;
  valueCurr[Metric_TrisDrawn]  += tris;
  valueCurr[Metric_VertsDrawn] += verts;
}

void Metric_AddDrawImm (int32 polys, int32 tris, int32 verts) {
  valueCurr[Metric_Immediate]  += 1;
  valueCurr[Metric_PolysDrawn] += polys;
  valueCurr[Metric_TrisDrawn]  += tris;
  valueCurr[Metric_VertsDrawn] += verts;
}

void Metric_Inc (Metric self) {
  valueCurr[self] += 1;
}

void Metric_Mod (Metric self, int32 delta) {
  valueCurr[self] += delta;
}

void Metric_Reset () {
  MemZero(valueCurr, sizeof(valueCurr));
}
