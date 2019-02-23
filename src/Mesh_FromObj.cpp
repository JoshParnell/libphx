#include "ArrayList.h"
#include "Mesh.h"
#include "PhxString.h"
#include "Vertex.h"

#include <errno.h>

/* TODO : See if it's cleaner to use sscanf */
/* TODO : Should the Mesh API have a mechanism for checking degenerate polygons? */

/* ROBUSTNESS : Support \ line continuations */
/* ROBUSTNESS : Fatal when there are extra elements on a line */

/* NOTE :
 *   - Fatal() on any error
 *   - Standard supports polygonal and free form objects
 *   - Coordinate system is right handed
 *   - Rational curves and surfaces have a w-coord in geometric vertices
 *   - Vertex normals supercede smoothing groups
 *   - Each vertex type can appear in multiple spacially disjoint groups
 *   - Negative index values indicate a relative index
 *   - Group, display, and render statements are stateful (remembered until changed)
 *   - Not validating mesh integrity. If desired, this should be done in a common place for all meshes.
 *
 *   SUPPORTED
 *   - #          comment
 *   - v          geometric vertice, 3-4 floats
 *   - vt         texture vertices, 1-3 floats, defaults to 0
 *   - vn         vertex normals
 *   - f          triangles / vertices
 *                  1-3 integers separated by '/' iv[/ivt/ivn],
 *                  3-4(?) sets of indices (triangle or quadrilateral)
 *                  "//" when skipping ivt
 *                  no spaces
 *                  slashes must be consistent across entire line
 *                  "There may be more than one series of geometric vertex/texture vertex/vertex normal numbers on a line." ???
 *
 *   UNSUPPORTED, NON-FAILING
 *   - p          point
 *   - l          line
 *   - g          grouping
 *                  may have multiple space delimited groups
 *                  letters, numbers, ???
 *                  defaults to "default"
 *   - s          smoothing group
 *                0 or "off" disable smoothing and are supported
 *                other values fail
 *   - o          object name
 *   - maplib
 *   - usemap
 *   - usemtl     material name
 *   - mtllib     material library
 *
 *   UNSUPPORTED, FAILING
 *   - vp         parameter space vertices
 *   - cstype     curve or surface type
 *   - deg        degree
 *   - bmat       basis matrix
 *   - step       step size
 *   - fo         face outline, deprecated in 2.11
 *
 *   - curv       curve
 *   - curve2     2D curve
 *   - surf       surface
 *
 *   - param      parameter values
 *   - trim       outer trimming loop
 *   - hole       inner trimming loop
 *   - scrv       special curve
 *   - sp         special point
 *   - end        end statement
 *
 *   - con        connect
 *
 *   - mg         merging group
 *
 *   - bevel      bevel interpolation
 *   - c_interp   color interoplation
 *   - d_interp   dissolve interpolation
 *   - lod        level of detail ***
 *   - shadow_obj shadow casting
 *   - trace_obj  ray tracing
 *   - ctech      curve approximation technique
 *   - stech      surface approximation technique
 *
 *   - bsp        superseded in 3.0
 *   - bzp        superseded in 3.0
 *   - cdc        superseded in 3.0
 *   - cdp        superseded in 3.0
 *   - res        superseded in 3.0
 */

struct ParseState {
  const char* cursor;
  const char* endOfData;
  const char* lineStart;
  int32 lineNumber;
};

static void Obj_Fatal (cstr message, ParseState* s) {
  int32 len = 0;
  const char* ch = s->lineStart;
  while (ch < s->endOfData && *ch != '\r' && *ch != '\n') {
    ch++;
    len += 1;
  }

  char* line = (char*) MemAlloc(len + 1);
  MemCpy(line, s->lineStart, len);
  line[len] = 0;

  Fatal("%s Line %i\n%s", message, s->lineNumber, line);
}

static bool ConsumeRestOfLine (ParseState* s) {
  const char* oldPosition = s->cursor;

  while (s->cursor < s->endOfData && *s->cursor != '\r' && *s->cursor != '\n')
    s->cursor++;

  int32 cr = 0;
  int32 nl = 0;
  while (s->cursor < s->endOfData && (*s->cursor == '\r' || *s->cursor == '\n')) {
    if (*s->cursor == '\r') {
      if (cr == 1) {
        cr = nl = 0;
        s->lineNumber++;
      }
      cr++;
    }
    if (*s->cursor == '\n') {
      if (nl == 1) {
        cr = nl = 0;
        s->lineNumber++;
      }
      nl++;
    }
    s->cursor++;
  }

  return s->cursor != oldPosition;
}

static bool ConsumeWhitespace (ParseState* s) {
  const char* oldPosition = s->cursor;

  while (s->cursor < s->endOfData && (*s->cursor == ' ' || *s->cursor == '\t'))
    s->cursor++;

  return s->cursor != oldPosition;
}

static bool ConsumeToken (char* token, int32 tokenLen, ParseState* s) {
  int32 i = 0;

  while (s->cursor < s->endOfData && i < tokenLen - 1
     && *s->cursor != ' '  && *s->cursor != '\t'
     && *s->cursor != '\r' && *s->cursor != '\n') {
    token[i++] = *s->cursor;
    s->cursor++;
  }
  token[i] = 0;

  return i != 0;
}

static bool ConsumeFloat (float* value, ParseState* s) {
  char* afterFloat;
  float f = strtof(s->cursor, &afterFloat);

  if (errno == ERANGE)
    Obj_Fatal("Parsed float in .obj data is out of range.", s);

  if (afterFloat != s->cursor) {
    s->cursor = afterFloat;
    *value = f;
    return true;
  }

  return false;
}

static bool ConsumeInt (int32* value, ParseState* s) {
  char* afterInt;
  int32 i = strtol(s->cursor, &afterInt, 10);

  if (errno == ERANGE)
    Obj_Fatal("Parsed int in .obj data is out of range.", s);

  if (afterInt != s->cursor) {
    s->cursor = afterInt;
    *value = i;
    return true;
  }

  return false;
}

static bool ConsumeCharacter (char character, ParseState* s) {
  if (s->cursor < s->endOfData && *s->cursor == character) {
    s->cursor++;
    return true;
  }

  return false;
}

Mesh* Mesh_FromObj (cstr bytes) {
  int bytesSize = StrLen(bytes);

  ParseState s = { 0 };
  s.cursor    = bytes;
  s.endOfData = s.cursor + bytesSize; //One past last

  Mesh* mesh = Mesh_Create();
  int32 vertexCount = 0;
  int32 indexCount  = 0;
  int32 faceCount   = 0;

  ArrayList(Vec3f, positions); ArrayList_Init(positions);
  ArrayList(Vec2f, uvs);       ArrayList_Init(uvs);
  ArrayList(Vec3f, normals);   ArrayList_Init(normals);

  /* OPTIMIZE : These heuristics could be made better by analyzing more meshes */
  /* NOTE : This makes no real difference right now. Bottleneck is elsewhere. */
  ArrayList_Reserve(positions, (int32) (.008f * bytesSize));
  ArrayList_Reserve(uvs,       (int32) (.008f * bytesSize));
  ArrayList_Reserve(normals,   (int32) (.008f * bytesSize));
  Mesh_ReserveIndexData(mesh,  (int32) (.050f * bytesSize));
  Mesh_ReserveVertexData(mesh, (int32) (.050f * bytesSize));

  while (true) {
    s.lineStart = s.cursor;
    s.lineNumber++;

    #define MAX_TOKEN_LEN 16
    char token[MAX_TOKEN_LEN];

    ConsumeWhitespace(&s);
    ConsumeToken(token, MAX_TOKEN_LEN, &s);
    ConsumeWhitespace(&s);

    if (StrEqual(token, "")) {
      //End of file
      if (s.cursor >= s.endOfData)
        break;

      //Blank line
    }
    //Geometric Vertex
    else if (StrEqual(token, "v")) {
      if (positions_size == INT32_MAX)
        Obj_Fatal(".obj data contains more vertex positions than will fit in an ArrayList.", &s);

      Vec3f p;
      if (!(ConsumeFloat(&p.x, &s) && ConsumeFloat(&p.y, &s) && ConsumeFloat(&p.z, &s)))
        Obj_Fatal("Failed to parse geometric vertex from .obj data.", &s);

      ArrayList_Append(positions, p);
    }
    //Texture Vertex
    else if (StrEqual(token, "vt")) {
      if (uvs_size == INT32_MAX)
        Obj_Fatal(".obj data contains more UVs than will fit in an ArrayList.", &s);

      Vec2f uv;
      if (!(ConsumeFloat(&uv.x, &s) && ConsumeFloat(&uv.y, &s)))
        Obj_Fatal("Failed to parse texture vertex from .obj data.", &s);

      ArrayList_Append(uvs, uv);
    }
    //Vertex Normal
    else if (StrEqual(token, "vn")) {
      if (normals_size == INT32_MAX)
        Obj_Fatal(".obj data contains more normals than will fit in an ArrayList.", &s);

      Vec3f n;
      if (!(ConsumeFloat(&n.x, &s) && ConsumeFloat(&n.y, &s) && ConsumeFloat(&n.z, &s)))
        Obj_Fatal("Failed to parse vertex normal from .obj data.", &s);

      ArrayList_Append(normals, n);
    }
    //Face
    else if (StrEqual(token, "f")) {
      struct VertexIndices
      {
        int32 iP;
        int32 iN;
        int32 iUV;
      };

      int32 vertexIndicesCount = 0;
      VertexIndices vertexIndices[4] = {};

      while (s.cursor < s.endOfData && *s.cursor != '\r' && *s.cursor != '\n') {
        VertexIndices* face = &vertexIndices[vertexIndicesCount];

        face->iUV = INT32_MIN;
        face->iN  = INT32_MIN;

        if (!ConsumeInt(&face->iP, &s))
          Obj_Fatal("Failed to parse face vertex index from .obj data.", &s);

        if (ConsumeCharacter('/', &s)) {
          ConsumeInt(&face->iUV, &s);

          if (ConsumeCharacter('/', &s)) {
            ConsumeInt(&face->iN, &s);
          }
        }
        vertexIndicesCount++;

        ConsumeWhitespace(&s);
      }

      for (int32 i = 0; i < vertexIndicesCount; i++) {
        if (vertexCount == INT32_MAX)
          Obj_Fatal(".obj data contains more vertex indices than will fit in an ArrayList.", &s);

        VertexIndices* face = &vertexIndices[i];
        Vertex vertex = {};

        face->iP += (face->iP < 0) ? positions_size : -1;
        if (face->iP < 0 || face->iP >= positions_size)
          Obj_Fatal("Face vertex index is out of range in .obj data", &s);
        vertex.p = positions_data[face->iP];

        if (face->iN != INT32_MIN) {
          face->iN += (face->iN < 0) ? normals_size : -1;
          if (face->iN < 0 || face->iN >= normals_size)
            Obj_Fatal("Face normal index is out of range in .obj data", &s);
          vertex.n = normals_data[face->iN];
        }

        if (face->iUV != INT32_MIN) {
          face->iUV += (face->iUV < 0) ? uvs_size : -1;
          if (face->iUV < 0 || face->iUV >= uvs_size)
            Obj_Fatal("Face UV index is out of range in .obj data", &s);
          vertex.uv = uvs_data[face->iUV];
        }

        vertexCount++;
        Mesh_AddVertexRaw(mesh, &vertex);
      }

      if (indexCount >= INT32_MAX - vertexIndicesCount)
        Obj_Fatal(".obj data contains more vertex indices than will fit in an ArrayList", &s);

      /* NOTE : Ensure the mesh doesn't have any degenerate polygons (i.e. where
       *        one of the vertex positions is equal to one or more other
       *        positions). */
      Vertex* vertices = Mesh_GetVertexData(mesh);
      int32 verticesLen = Mesh_GetVertexCount(mesh);
      for (int32 i = 0; i < vertexIndicesCount; i++) {
        for (int32 j = i + 1; j < vertexIndicesCount; j++) {
          Vec3f p1 = vertices[verticesLen - vertexIndicesCount + i].p;
          Vec3f p2 = vertices[verticesLen - vertexIndicesCount + j].p;
          if (Vec3f_Equal(p1, p2))
            Obj_Fatal(".obj data contains a degenerate polygon.", &s);
        }
      }

      if (vertexIndicesCount == 3) {
        faceCount  += 1;
        indexCount += vertexIndicesCount;
        Mesh_AddTri(mesh, vertexCount - 3, vertexCount - 2, vertexCount - 1);
      }
      else if (vertexIndicesCount == 4) {
        faceCount  += 2;
        indexCount += vertexIndicesCount;
        Mesh_AddQuad(mesh, vertexCount - 4, vertexCount - 3, vertexCount - 2, vertexCount - 1);
      }
      else {
        Obj_Fatal(".obj data has an unexpected number of vertices in a face", &s);
      }
    }
    //Unsupported
    else {
      //Non-fatal
      if (StrEqual(token, "#"     )
       || StrEqual(token, "f"     )
       || StrEqual(token, "s"     )
       || StrEqual(token, "p"     )
       || StrEqual(token, "l"     )
       || StrEqual(token, "g"     )
       || StrEqual(token, "o"     )
       || StrEqual(token, "maplib")
       || StrEqual(token, "usemap")
       || StrEqual(token, "usemtl")
       || StrEqual(token, "mtllib")) {
        //Do nothing
      }
      //Fatal
      else {
        Obj_Fatal("Unsupported token in .obj data.", &s);
      }
    }

    ConsumeRestOfLine(&s);
  }

  ArrayList_Free(positions);
  ArrayList_Free(uvs);
  ArrayList_Free(normals);

  return mesh;
}
