#include "Array.h"
#include "BSP.h"
#include "Plane.h"
#include "Sphere.h"

#include <float.h>

/* Adam's Stupidly Fast BSP Implementation
 *
 * Implementation Details
 * - We refer to internal tree nodes as 'nodes', and leaf nodes as 'leaves'.
 * - BSP.nodes contains the internal nodes (and *not* any leaf nodes).
 * - BSP.triangles contains all the final (split) triangles.
 * - We don't store leaf nodes at all. Nodes use a negative index to indicate
 *     the target is a leaf. This index is relative to BSP.triangles. The node
 *     also stores a triangleCount.
 * - No leaves are actually stored at all. Instead, nodes store indices to a flat triangle list.
 * - At 4 triangles per leaf, 67% of the tree size is the actual triangles.
 * - At 32 triangles per leaf triangles are 88% of the tree.
 * - Thus, the only ways to significantly reduce tree size are to either choose
 *     splits that cut fewer triangles or store references to the original mesh
 *     vertices (or triangles, but care would have to be taken to handle new
 *     triangles formed by cutting).
 *
 * TODO
 * Build Speed Optimizations
 *  There's not much obvious waste in the build process, so there's not enough
 *  'low-hanging fruit' to reduce the build time by more than 10-20%.
 *  For reference, 98% of the time is spent in in BSPBuild_CreateNode (as
 *  opposed to the prologue or epilogue in BSP_Create). Of that, 72% of the
 *  total time is spend in BSPBuild_ChooseSplitPlane and 24% is spent splitting
 *  polygons to front and back lists. This makes is clear that the only place
 *  worth spending optimization time right now is BSPBuild_ChooseSplitPlane.
 *
 *  To make a major difference in speed consider the following approaches:
 *    1) Stop splitting triangles at all
 *       Short of fundammentally improving the plane selection algorithm, I
 *       think this is the most promising optimization and should be done before
 *       any others.
 *    2) Improve the core plane selection algorithm (do general splits!!).
 *    3) Find a way to make BSPBuild_CreateNode iterative instead of recursive.
 *
 * Once those are d one consider the following smaller optimizations:
 *    1) Early out of ScoreSplittingPlane
 *       This requires deriving a way to predict when the best possible score is
 *       worse than the current best score. This is a constrained, multivariate
 *       optimization problem.
 *    2) Remember the result of Plane_ClassifyPolygon when returning from
 *       ChooseSplitPlane.
 *       I saw an 8% gain by doing this.
 *    3) Store planes with triangles
 *    4) Reuse the incoming polygon list in BSPBuild_CreateNode
 *
 * - Replace BSP_Create with BSP_Create(Polygon* polygons, int32 polygonsLen) and BSP_FromMesh(Mesh*)
 * - Just go back to using positive indices and check triangleCount instead.
 * - Need profiling for the tree build time and max memory used.
 * - Implement hinting planes.
 * - Consider triangles in mesh.
 * - The build process is using a *lot* of temporary memory. This will need to be addressed.
 * - Probably want to use a stack based scratch allocator when building the tree.
 * - Leaf storing or node storing? (Put unsplit triangles in nodes instead of passing to both sides?)
 * - Get rid of the Mesh -> Polygon conversion in BSP_Create?
 * - Store internal nodes like a breadth first search.
 * - Prefetch nodes or refactor to remove data dependency (pack nodes and index into an array)
 * - Make it thread safe
 */

/* Performance Data
 *   (Currently biased for hits. Just getting structure together.)
 *   Each test is run with the 300,000 rays. Rays are the same for individual tests, different when varying a parameter
 * |---------------------------------------------------------------------------------------------------------------------------------------------------|
 * |          |         |      |  BSP  |  MiB  |           |  Tri  |           |         |  Max  | Avg Leaf ||  Avg   |  Avg   |  Avg  |  Avg  | Avg   |
 * | Mesh     |  Tris   | MiB  |  MiB  | Ratio |   Tris    | Ratio |   Nodes   | Leaves  | Depth |  Depth   || Ray us | Leaves | Nodes | Depth | Tris  |
 * |-------------------------------------------------------------------------------------------------------------------------------------------|-------|
 * | luffa    | 327,680 | 30.0 | 119.2 | 3.97  | 1,350,532 | 4.12  | 1,240,357 | 620,179 |  736  |   97.3   ||  21.8  |  25.8  | 301.2 | 107.3 |  60.8 |  Halfway split poly, 1 poly/leaf
 * |          |         |      | 117.0 | 3.90  | 1,286,089 | 3.92  | 1,241,483 | 620,742 |  956  |   41.1   ||  16.2  |  20.5  | 181.3 |  50.4 |  59.1 |  Random split poly,  1 poly/leaf
 * |          |         |      |  89.6 | 2.99  | 1,286,736 | 3.93  |   781,263 | 390,632 |  807  |   45.2   ||  16.4  |  16.8  | 162.0 |  50.5 |  83.8 |  Random split poly,  4 poly/leaf
 * |          |         |      |       |       |           |       |           |         |       |          ||        |        |       |       |       |
 * |          |         |      |  83.2 | 2.77  |   881,460 | 2.69  |   913,521 | 456,761 |  627  |   33.1   ||  13.6  |  17.8  | 151.8 |  35.6 |  45.3 |  Best of 10, k=0.8,  1 poly/leaf
 * |          |         |      |  43.2 | 1.44  |   822,198 | 2.51  |   260,919 | 130,460 |  497  |   49.3   ||  18.7  |  11.2  | 106.5 |  36.6 | 195.2 |  Best of 10, k=0.8, 32 tris/leaf
 * |          |         |      |  49.5 | 1.65  |   858,619 | 2.62  |   349,793 | 174,897 |  395  |   40.3   ||  16.6  |  12.4  | 115.1 |  33.5 | 122.6 |  Best of 10, k=0.8, 16 tris/leaf
 * |          |         |      |  57.0 | 1.90  |   873,860 | 2.67  |   471,915 | 235,958 |  385  |   37.7   ||  15.2  |  13.6  | 126.5 |  36.0 |  85.0 |  Best of 10, k=0.8,  8 tris/leaf
 * |          |         |      |  68.8 | 2.29  |   881,320 | 2.69  |   670,345 | 335,173 |  491  |   36.0   ||  14.6  |  15.6  | 139.8 |  38.3 |  64.6 |  Best of 10, k=0.8,  4 tris/leaf **
 * |          |         |      |  79.1 | 2.64  |   877,302 | 2.68  |   847,087 | 423,544 |  626  |   34.6   ||  14.5  |  17.0  | 150.5 |  39.2 |  52.2 |  Best of 10, k=0.8,  2 tris/leaf
 * |          |         |      |  84.0 | 2.80  |   880,643 | 2.69  |   927,327 | 463,664 |  586  |   33.8   ||  14.5  |  18.1  | 156.3 |  37.9 |  46.7 |  Best of 10, k=0.8,  1 tris/leaf
 * |          |         |      |       |       |           |       |           |         |       |          ||        |        |       |       |       |
 * |          |         |      | 110.8 | 3.69  | 1,445,775 | 4.41  | 1,044,187 | 522,094 |  676  |   27.5   ||  13.0  |  20.2  | 159.9 |  29.6 |  73.0 |  Best of 10, k=0.00, 4 tris/leaf
 * |          |         |      | 108.0 | 3.60  | 1,405,199 | 4.29  | 1,020,113 | 510,057 |  527  |   26.9   ||  12.7  |  19.7  | 151.8 |  27.8 |  70.1 |  Best of 10, k=0.05, 4 tris/leaf
 * |          |         |      | 107.4 | 3.58  | 1,397,855 | 4.27  | 1,014,775 | 507,388 |  518  |   27.3   ||  12.7  |  19.4  | 148.7 |  28.9 |  70.8 |  Best of 10, k=0.10, 4 tris/leaf
 * |          |         |      | 106.5 | 3.55  | 1,385,452 | 4.23  | 1,007,605 | 503,803 |  604  |   27.1   ||  12.9  |  20.3  | 155.0 |  28.3 |  71.9 |  Best of 10, k=0.15, 4 tris/leaf
 * |          |         |      | 105.6 | 3.52  | 1,371,420 | 4.19  | 1,000,279 | 500,140 |  600  |   27.4   ||  12.7  |  19.4  | 151.2 |  28.4 |  70.4 |  Best of 10, k=0.20, 4 tris/leaf
 * |          |         |      | 103.9 | 3.46  | 1,347,734 | 4.11  |   984,729 | 492,365 |  510  |   27.6   ||  12.6  |  19.6  | 152.0 |  28.4 |  70.3 |  Best of 10, k=0.25, 4 tris/leaf
 * |          |         |      | 102.8 | 3.43  | 1,333,958 | 4.07  |   974,447 | 487,224 |  441  |   28.0   ||  12.7  |  19.5  | 152.8 |  29.6 |  72.1 |  Best of 10, k=0.30, 4 tris/leaf
 * |          |         |      | 101.8 | 3.39  | 1,319,483 | 4.03  |   966,787 | 483,394 |  544  |   27.1   ||  12.1  |  19.5  | 148.7 |  27.3 |  68.0 |  Best of 10, k=0.35, 4 tris/leaf
 * |          |         |      | 100.4 | 3.35  | 1,301,040 | 3.97  |   954,015 | 477,008 |  578  |   27.7   ||  12.3  |  18.5  | 143.7 |  28.9 |  68.9 |  Best of 10, k=0.40, 4 tris/leaf
 * |          |         |      |  99.1 | 3.30  | 1,282,398 | 3.91  |   943,627 | 471,814 |  614  |   28.5   ||  12.4  |  18.6  | 148.3 |  31.0 |  72.5 |  Best of 10, k=0.55, 4 tris/leaf
 * |          |         |      |  94.6 | 3.15  | 1,224,627 | 3.74  |   902,405 | 451,203 |  617  |   27.9   ||  11.7  |  18.5  | 143.6 |  28.4 |  67.7 |  Best of 10, k=0.50, 4 tris/leaf
 * |          |         |      |  89.7 | 2.99  | 1,159,800 | 3.54  |   856,997 | 428,499 |  774  |   29.4   ||  11.9  |  18.1  | 143.6 |  31.1 |  70.7 |  Best of 10, k=0.55, 4 tris/leaf
 * |          |         |      |  87.4 | 2.91  | 1,130,018 | 3.45  |   836,803 | 418,402 |  592  |   29.1   ||  11.6  |  18.1  | 144.7 |  30.2 |  68.1 |  Best of 10, k=0.60, 4 tris/leaf
 * |          |         |      |  84.0 | 2.80  | 1,085,805 | 3.31  |   805,335 | 402,668 |  543  |   29.4   ||  11.4  |  17.7  | 142.1 |  29.8 |  67.2 |  Best of 10, k=0.65, 4 tris/leaf
 * |          |         |      |  79.3 | 2.64  | 1,023,042 | 3.12  |   764,007 | 382,004 |  623  |   30.5   ||  11.0  |  16.3  | 136.8 |  31.1 |  64.8 |  Best of 10, k=0.70, 4 tris/leaf
 * |          |         |      |  74.1 | 2.47  |   953,484 | 2.91  |   717,337 | 358,669 |  597  |   32.1   ||  10.6  |  16.0  | 135.3 |  33.2 |  63.8 |  Best of 10, k=0.75, 4 tris/leaf
 * |          |         |      |  68.5 | 2.28  |   878,411 | 2.68  |   667,837 | 333,919 |  465  |   36.0   ||  10.6  |  15.3  | 136.9 |  38.0 |  64.5 |  Best of 10, k=0.80, 4 tris/leaf
 * |          |         |      |  64.2 | 2.14  |   817,670 | 2.50  |   630,563 | 315,282 |  512  |   42.1   ||  10.6  |  14.9  | 145.5 |  44.9 |  62.7 |  Best of 10, k=0.85, 4 tris/leaf **
 * |          |         |      |  61.6 | 2.05  |   781,461 | 2.38  |   606,957 | 303,479 |  657  |   63.3   ||  11.5  |  14.7  | 170.6 |  70.9 |  69.4 |  Best of 10, k=0.90, 4 tris/leaf
 * |          |         |      |  62.4 | 2.08  |   794,154 | 2.42  |   613,591 | 306,796 |  837  |  124.4   ||  15.8  |  15.8  | 271.3 | 143.9 |  99.0 |  Best of 10, k=0.95, 4 tris/leaf
 * |          |         |      |  68.4 | 2.28  |   867,644 | 2.65  |   672,997 | 336,499 | 1057  |  205.7   ||  18.8  |  19.5  | 350.3 | 232.1 |  92.0 |  Best of 10, k=1.00, 4 tris/leaf
 * |          |         |      |       |       |           |       |           |         |       |          ||        |        |       |       |       |
 * |          |         |      |  64.3 | 2.14  |   820,378 | 2.50  |   631,519 | 315,760 |  406  |   42.2   ||  15.3  |  15.0  | 145.4 |  45.1 |  63.4 |  Best of  10, k=0.85, 4 tris/leaf, New baseline
 * |          |         |      |  60.4 | 2.01  |   763,716 | 2.33  |   596,715 | 298,358 |  438  |   36.2   ||  13.5  |  15.0  | 130.5 |  39.1 |  61.5 |  Best of 100
 * |          |         |      |  45.0 | 1.50  |   820,378 | 2.50  |   631,519 | 315,760 |  406  |   42.2   ||  14.0  |  15.0  | 145.4 |  45.1 |  63.4 |  Best of  10, removed cruft
 * |          |         |      |  45.0 | 1.50  |   820,378 | 2.50  |   631,519 | 315,760 |  406  |   42.2   ||  12.7  |  15.0  | 145.4 |  45.1 |  63.4 |  Share empty leaves, NOTE: We're now counting nodes incorrectly
 * |          |         |      |  45.0 | 1.50  |   820,378 | 2.50  |   631,519 | 315,760 |  406  |   42.2   ||  11.7  |  15.0  | 145.4 |  45.1 |  63.4 |  Put nodes in an array
 * |          |         |      |  45.0 | 1.50  |   820,378 | 2.50  |   631,519 | 315,760 |  406  |   42.2   ||  10.8  |  11.3  | 139.2 |  46.6 |  56.3 |  Reduce ray epsilon from 11x plane to 2x (just to see)
 * |          |         |      |       |       |           |       |           |         |       |          ||        |        |       |       |       |
 * |          |         |      |  45.0 | 1.50  |   820,378 | 2.50  |   631,519 | 315,760 |  406  |   42.2   ||  11.8  |  15.0  | 145.4 |  45.1 |  63.4 |  New baseline
 * |          |         |      |  45.0 | 1.50  |   820,378 | 2.50  |   631,519 | 315,760 |  406  |   42.2   ||  13.0  |  20.0  | 154.4 |  43.2 |  74.1 |  Collapse and correct plane checks
 * |          |         |      |  45.0 | 1.50  |   820,378 | 2.50  |   631,519 | 315,760 |  406  |   42.2   ||  13.2  |  17.3  | 149.5 |  44.1 |  68.1 |  Reduce epsilon from 11x to 8x (still correct)
 * |          |         |      |  45.0 | 1.50  |   820,378 | 2.50  |   631,519 | 315,760 |  406  |   42.2   ||  11.1  |  12.0  | 139.9 |  46.3 |  56.8 |  Reduce epsilon 2x (just to see)
 * |          |         |      |  45.0 | 1.50  |   820,378 | 2.50  |   631,519 | 315,760 |  406  |   42.2   ||  17.8  |  16.3  | 233.6 |  28.3 | 184.4 |  Reduce epsilon 0 (just to see) (almost all rays are missing) I think something here is broken.
 * |          |         |      |  45.0 | 1.50  |   820,378 | 2.50  |   631,519 | 315,760 |  406  |   42.2   ||  12.3  |  16.1  | 147.9 |  44.8 |  66.3 |  Back to 8x, Clamp splits with Min/Max
 * |          |         |      |  45.0 | 1.50  |   820,378 | 2.50  |   631,519 | 315,760 |  406  |   42.2   ||  12.4  |  16.1  | 147.9 |  44.8 |  66.3 |  Clamp splits with Lerp
 * |          |         |      |  45.0 | 1.50  |   820,378 | 2.50  |   631,519 | 315,760 |  406  |   42.2   ||  12.4  |  16.1  | 147.9 |  44.8 |  66.3 |  Clamp splits with ternary
 * |          |         |      |  45.0 | 1.50  |   820,378 | 2.50  |   631,519 | 315,760 |  406  |   42.2   ||  12.6  |  16.1  | 147.9 |  44.8 |  66.3 |  Check both sides when ray is parallel
 * |          |         |      |  45.0 | 1.50  |   820,378 | 2.50  |   631,519 | 315,760 |  406  |   42.2   ||  12.5  |  16.1  | 147.9 |  44.8 |  66.3 |  Stupid if restructuring
 * |          |         |      |  45.0 | 1.50  |   820,378 | 2.50  |   631,519 | 315,760 |  406  |   42.2   ||  11.2  |  16.1  | 147.9 |  44.8 |  66.3 |  Remove Asserts
 * |          |         |      |  45.0 | 1.50  |   820,378 | 2.50  |   631,519 | 315,760 |  406  |   42.2   ||  10.3  |  16.1  | 147.9 |  44.8 |  66.3 |  Don't bother with centroid for Triangle_To
 * |          |         |      |  45.0 | 1.50  |   820,378 | 2.50  |   631,519 | 315,760 |  406  |   42.2   ||   8.7  |  16.1  | 147.9 |  44.8 |  66.3 |  Disable....vsync?!?!?!?!
 * |          |         |      |       |       |           |       |           |         |       |          ||        |        |       |       |       |
 * |          |         |      |  45.0 | 1.50  |   820,378 | 2.50  |   631,519 | 315,760 |  406  |   42.2   ||   8.8  |  16.1  | 147.9 |  44.8 |  66.3 |  New baseline
 * |          |         |      |  44.9 | 1.50  |   818,235 | 2.50  |   629,781 | 314,891 |  518  |   43.7   ||   8.8  |  15.4  | 147.8 |  47.5 |  68.5 |  Build back nodes before front nodes (changes the RNG)
 * |          |         |      |  44.9 | 1.50  |   818,235 | 2.50  |   314,891 | 191,009 |  518  |   43.7   ||   8.8  |  15.4  | 147.8 |  47.5 |  68.5 |  Stop counting leaves with nodes, and empty leaves with leaves
 * |          |         |      |  44.9 | 1.50  |   818,235 | 2.50  |   314,891 | 191,009 |  518  |   43.7   ||   9.2  |  15.4  | 132.4 |  47.5 |  68.5 |  Reference nodes by index instead of pointer
 * |          |         |      |       |       |           |       |           |         |       |          ||        |        |       |       |       |
 * | asteroid |         |      |   0.6 | 11.97 |    10,140 | 3.13  |     4,140 |   2,569 |   52  |   17.6   ||   2.5  |   8.8  |  40.9 |  14.3 |  17.1 |  These are very haphazard. I don't fully trust the numbers
 * | station  |         |      |  11.9 |  4.24 |   306,362 | 1.89  |    26,376 |   9,649 |  136  |   30.5   ||   9.5  |  15.8  |  79.1 |  27.3 | 240.1 |
 * |          |         |      |       |       |           |       |           |         |       |          ||        |        |       |       |       |
 * | luffa    |         |      |  44.9 | 1.50  |   818,235 | 2.50  |   314,891 | 191,009 |  518  |   43.7   ||   9.1  |  15.4  | 132.4 |  47.5 |  68.5 |  Use a struct to pass data when building tree nodes.
 * |          |         |      |  45.1 | 1.50  |   821,481 | 2.51  |   315,963 | 191,669 |  578  |   42.1   ||   9.1  |  15.9  | 132.3 |  45.2 |  66.0 |  Don't search for a split plane when we already have few enough triangles (changes the RNG)
 * |          |         |      | 166.7 | 5.56  | 2,929,218 | 8.94  | 1,143,003 | 976,642 |  591  |   80.8   ||   9.0  |  16.3  | 137.9 |  47.8 |  28.0 |  Random bounding box cut when out of valid polygons to split with
 * |          |         |      |  45.1 | 1.50  |   821,481 | 2.51  |   315,963 | 191,669 |  578  |   42.1   ||  10.0  |  15.9  | 132.3 |  45.2 |  66.0 |  Wasn't normalizing n in Triangle_ToPlane
 * |          |         |      |  60.8 | 2.03  | 1,011,646 | 3.09  |   487,869 | 364,063 |  516  |   61.1   ||   9.0  |  16.1  | 136.4 |  49.1 |  27.4 |  Ensure leaves *never* have more than MAX_LEAF_SIZE triangles
 * |          |         |      |  70.3 | 2.34  |   842,267 | 2.57  |   766,160 | 309,795 |  563  |   96.2   ||   8.7  |  16.1  | 139.4 |  52.7 |  26.6 |  Vary k with tree depth 0.85/200 -> 0.00/1000
 * |          |         |      | 103.8 | 3.46  |   846,084 | 2.58  | 1,331,922 | 311,220 |  718  |  194.5   ||   9.3  |  16.3  | 152.9 |  64.2 |  26.1 |  Vary k with tree depth 0.85/400 -> 0.00/1000
 * |          |         |      | 143.1 | 4.77  |   844,388 | 2.58  | 1,986,942 | 310,904 |  721  |  296.8   ||   9.0  |  16.6  | 158.2 |  70.0 |  26.7 |  Vary k with tree depth 0.85/600 -> 0.00/1000
 * |          |         |      |  60.5 | 2.02  |   846,826 | 2.58  |   587,894 | 310,875 |  562  |   68.0   ||   9.1  |  15.8  | 137.1 |  50.0 |  26.3 |  Vary k with tree depth 0.85/100 -> 0.50/1000
 * |          |         |      |  58.3 | 1.94  |   846,096 | 2.58  |   547,737 | 310,575 |  522  |   62.6   ||   8.6  |  15.9  | 135.0 |  50.1 |  26.3 |  Vary k with tree depth 0.85/100 -> 0.25/1000
 * |          |         |      |       |       |           |       |           |         |       |          ||        |        |       |       |       |
 * |          |         |      |  58.5 | 1.95  |   849,718 | 2.59  |   549,291 | 311,727 |  672  |   62.8   ||   9.0  |  16.0  | 135.3 |  50.4 |  26.3 |  Added centroid back to Triangle_ToPlane
 * |          |         |      |  58.5 | 1.95  |   849,718 | 2.59  |   549,291 | 311,727 |  672  |   62.8   ||   8.7  |  16.0  | 135.3 |  50.4 |  26.3 |  Use Triangle_ToPlaneFast in intersection test (lot of variance here, seeing as low as 8.1 us)
 * |          |         |      |  58.5 | 1.95  |   849,719 | 2.59  |   549,291 | 311,727 |  672  |   62.8   ||   8.3  |  16.0  | 135.3 |  50.4 |  26.3 |  Another sample
 * |          |         |      |  58.5 | 1.95  |   849,719 | 2.59  |   549,291 | 311,727 |  672  |   62.8   ||   8.2  |  16.0  | 135.3 |  50.4 |  26.3 |  Store triangles in a single big array
 * |          |         |      |  29.7 | 0.99  |   849,721 | 2.59  |   549,291 | 549,291 |  672  |   62.8   ||   7.8  |  16.0  | 135.3 |  50.4 |  26.3 |  Don't store leaves at all
 * |          |         |      |  29.2 | 0.97  |   841,756 | 2.57  |   309,857 | 309,857 |  582  |   59.8   ||   7.9  |  13.6  | 122.9 |  47.7 |  47.9 |  8 tris/leaf
 * |          |         |      |  38.4 | 1.28  |   841,756 | 2.57  |   309,857 | 309,857 |  582  |   59.8   ||   8.1  |  13.6  | 122.9 |  47.7 |  47.9 |  Fixed BSP size calculation
 * |          |         |      |       |       |           |       |           |         |       |          ||        |        |       |       |       |
 * |          |         |      |  45.9 | 1.53  |   849,721 | 2.59  |   549,291 | 549,291 |  672  |   62.8   ||   8.1  |  16.0  | 135.3 |  50.4 |  26.3 |   4 tris/leaf
 * |          |         |      |  38.4 | 1.28  |   841,756 | 2.57  |   309,857 | 309,857 |  582  |   59.8   ||   8.2  |  13.6  | 122.9 |  47.7 |  47.9 |   8 tris/leaf
 * |          |         |      |  33.9 | 1.13  |   822,172 | 2.51  |   185,560 | 185,560 |  483  |   56.1   ||   8.5  |  12.6  | 115.9 |  46.6 |  85.9 |  16 tris/leaf
 * |          |         |      |  31.3 | 1.04  |   785,255 | 2.40  |   140,767 | 140,767 |  428  |   57.0   ||   9.2  |  11.3  | 101.4 |  43.3 | 146.1 |  32 tris/leaf
 * |          |         |      |       |       |           |       |           |         |       |          ||        |        |       |       |       |
 * |          |         |      |  31.3 | 1.04  |   785,255 | 2.40  |   140,767 | 140,767 |  428  |   57.0   ||   7.4  |  11.3  | 101.4 |  55.9 | 146.0 |  32 tris/leaf, moved profiling code out of loop, rays now always the same (started getting way too much variation), fixed avg depth calculation
 * |          |         |      |  31.3 | 1.04  |   785,255 | 2.40  |   140,767 | 140,767 |  428  |   57.0   ||   6.9  |  11.3  | 101.5 |  55.9 | 146.3 |  Moller's ray-triangle test
 * |          |         |      |  33.9 | 1.13  |   822,172 | 2.51  |   185,560 | 185,560 |  483  |   56.1   ||   6.3  |  12.7  | 116.2 |  62.2 |  86.2 |  16 tris/leaf
 * |          |         |      |  34.4 | 1.15  |   827,393 | 2.53  |   197,842 | 197,842 |  474  |   53.5   ||   6.2  |  12.9  | 115.9 |  60.4 |  78.2 |  14 tris/leaf
 * |          |         |      |  35.0 | 1.17  |   831,193 | 2.54  |   212,601 | 212,601 |  649  |   54.9   ||   6.1  |  13.3  | 118.7 |  62.0 |  67.5 |  12 tris/leaf **
 * |          |         |      |  36.0 | 1.20  |   836,718 | 2.55  |   237,234 | 237,234 |  623  |   57.0   ||   6.1  |  13.7  | 122.5 |  63.5 |  59.9 |  10 tris/leaf
 * |          |         |      |  38.4 | 1.28  |   841,756 | 2.57  |   309,857 | 309,857 |  582  |   59.8   ||   5.9  |  13.7  | 123.2 |  64.4 |  48.1 |   8 tris/leaf
 * |          |         |      |  45.9 | 1.53  |   849,721 | 2.59  |   549,291 | 549,291 |  672  |   62.8   ||   6.0  |  16.1  | 135.6 |  67.4 |  26.5 |   4 tris/leaf
 * |          |         |      |       |       |           |       |           |         |       |          ||        |        |       |       |       |
 * |          |         |      |  35.0 | 1.17  |   831,193 | 2.54  |   212,601 | 212,601 |  649  |   54.9   ||   6.6  |  13.3  | 118.6 |  62.0 |  67.3 |  Barycentric, inline
 * |          |         |      |  35.0 | 1.17  |   831,193 | 2.54  |   212,601 | 212,601 |  649  |   54.9   ||   6.6  |  13.3  | 118.6 |  62.0 |  67.3 |  Barycentric, function
 * |          |         |      |  35.0 | 1.17  |   831,193 | 2.54  |   212,601 | 212,601 |  649  |   54.9   ||   6.2  |  13.3  | 118.7 |  62.0 |  67.5 |  Moller 1, inline
 * |          |         |      |  35.0 | 1.17  |   831,193 | 2.54  |   212,601 | 212,601 |  649  |   54.9   ||   6.2  |  13.3  | 118.7 |  62.0 |  67.5 |  Moller 1, function **
 * |          |         |      |  35.0 | 1.17  |   831,193 | 2.54  |   212,601 | 212,601 |  649  |   54.9   ||   6.4  |  13.3  | 118.6 |  62.0 |  67.3 |  Moller 2, inline
 * |          |         |      |  35.0 | 1.17  |   831,193 | 2.54  |   212,601 | 212,601 |  649  |   54.9   ||   6.4  |  13.3  | 118.6 |  62.0 |  67.3 |  Moller 2, function
 * |          |         |      |       |       |           |       |           |         |       |          ||        |        |       |       |       |
 * |          |         |      |       |       |           |       |           |         |       |          ||        |        |       |       |       |
 * |          |         |      |  35.0 | 1.17  |   831,193 | 2.54  |   212,601 | 212,601 |  649  |   54.9   ||   6.6  |  13.3  | 118.7 |  62.0 |  67.5 | Ray test baseline
 * |          |         |      |  35.0 | 1.17  |   831,193 | 2.54  |   212,601 | 212,601 |  649  |   54.9   ||  43.3  | 267.5  | 369.0 |  46.0 |1009.8 | Sphere test 8x plane epsilon
 * |          |         |      |  35.0 | 1.17  |   831,193 | 2.54  |   212,601 | 212,601 |  649  |   54.9   ||  42.8  | 263.5  | 364.1 |  45.7 | 995.1 | 2x plane epsilon
 * |          |         |      |       |       |           |       |           |         |       |          ||        |        |       |       |       |
 * |          |         |      |  35.0 | 1.17  |   831,193 | 2.54  |   212,601 | 212,601 |  649  |   54.9   ||   7.6  |  13.4  | 117.4 |  61.4 |  66.9 | Ray
 * |          |         |      |  35.0 | 1.17  |   831,193 | 2.54  |   212,601 | 212,601 |  649  |   54.9   ||  44.0  | 268.5  | 370.0 |  45.4 |1016.4 | Sphere r[0.05, 0.30]
 * |          |         |      |  35.0 | 1.17  |   831,193 | 2.54  |   212,601 | 212,601 |  649  |   54.9   ||   8.8  |  13.3  | 116.6 |  60.9 |  66.5 | Ray
 * |          |         |      |  35.0 | 1.17  |   831,193 | 2.54  |   212,601 | 212,601 |  649  |   54.9   ||  10.1  |  44.1  |  97.1 |  35.3 | 168.7 | Sphere r[0.01, 0.10]
 * |          |         |      |       |       |           |       |           |         |       |          ||        |        |       |       |       |
 * |          |         |      |  35.0 | 1.17  |   831,193 | 2.54  |   212,601 | 212,601 |  649  |   54.9   ||  41.4  | 263.5  | 364.1 |  45.7 | 995.1 | Sphere r[0.05, 0.30], 300,000, closest tri
 * |          |         |      |  35.0 | 1.17  |   831,193 | 2.54  |   212,601 | 212,601 |  649  |   54.9   ||  43.8  | 263.5  | 364.1 |  45.7 | 991.9 | Sphere r[0.05, 0.30], 300,000, first tri
 * |          |         |      |  30.2 | 1.01  |   748,050 | 2.28  |   148,262 | 148,262 |  594  |   77.0   ||  36.7  | 175.3  | 270.3 |  51.6 | 930.2 | 32 tris/leaf, 0.25-0.9 split cost
 * |          |         |      |  27.6 | 0.92  |   699,925 | 2.14  |   116,283 | 116,283 |  525  |   79.1   ||  39.8  | 154.1  | 250.3 |  53.6 |1089.3 | 64 tris/leaf, 0.25-0.9 split cost
 * |          |         |      |  27.6 | 0.92  |   699,347 | 2.13  |   116,176 | 116,176 |  471  |   81.5   ||  39.2  | 158.4  | 254.8 |  55.7 |1078.4 | 0.7-0.9 split cost
 * |          |         |      |  28.2 | 0.94  |   730,415 | 2.23  |   102,906 | 102,906 |  526  |   66.5   ||  41.9  | 141.3  | 212.5 |  38.4 |1187.3 | 0.85 split cost
 * |          |         |      |  35.0 | 1.17  |   831,193 | 2.54  |   212,601 | 212,601 |  649  |   54.9   ||   6.7  |  13.3  | 118.7 |  62.0 |  67.5 | Ray, 12 tri/leaf
 * |          |         |      |  31.3 | 1.04  |   785,255 | 2.40  |   140,767 | 140,767 |  428  |   57.0   ||   7.8  |  11.3  | 101.5 |  55.9 | 146.3 | Ray, 32 tri/leaf
 * |          |         |      |  28.2 | 0.94  |   729,757 | 2.23  |   102,917 | 102,917 |  553  |   62.5   ||   9.7  |  10.1  |  86.9 |  51.3 | 261.6 | Ray, 64 tri/leaf
 * |          |         |      |  24.7 | 0.82  |   666,388 | 2.03  |    60,420 |  60,420 |  711  |   81.5   ||  13.3  |   8.5  |  71.0 |  44.9 | 471.0 | Ray, 128 tri/leaf
 * |          |         |      |  35.0 | 1.17  |   831,193 | 2.54  |   212,601 | 212,601 |  649  |   54.9   ||  43.6  | 263.5  | 364.1 |  45.7 | 991.9 | Sphere, 12 tri/leaf
 * |          |         |      |  31.3 | 1.04  |   785,255 | 2.40  |   140,767 | 140,767 |  428  |   57.0   ||  45.1  | 195.8  | 284.8 |  42.4 |1160.1 | Sphere, 32 tri/leaf
 * |          |         |      |  28.2 | 0.94  |   729,757 | 2.23  |   102,917 | 102,917 |  553  |   62.5   ||  42.7  | 146.4  | 219.7 |  39.0 |1206.1 | Sphere, 64 tri/leaf
 * |          |         |      |  24.7 | 0.82  |   666,388 | 2.03  |    60,420 |  60,420 |  711  |   81.5   ||  41.0  |  85.0  | 140.4 |  34.0 |1306.7 | Sphere, 128 tri/leaf
 * |          |         |      |       |       |           |       |           |         |       |          ||        |        |       |       |       |
 * |          |         |      |  50.2 | 1.67  | 1,202,316 | 3.67  |   293,954 | 293,954 |  521  |   61.0   ||   5.6  |  10.5  | 129.7 |  69.6 |  58.0 | Crash fix and reused stack during intersection tests (NEW LAPTOP!!)
 * |          |         |      |  50.2 | 1.67  | 1,202,316 | 3.67  |   293,954 | 293,954 |  521  |   61.0   ||   5.5  |  10.5  | 129.7 |  69.6 |  58.0 | decompose on,  edge split on,   232 oversized leaves, avg  4.9 extra triangles.
 * |          |         |      |  47.0 | 1.57  | 1,129,048 | 3.45  |   269,534 | 269,534 |  451  |   52.6   ||   5.7  |  10.5  | 125.6 |  66.0 |  66.1 | decompose on,  edge split off, 5871 oversized leaves, avg 14.0 extra triangles.
 * |          |         |      |  51.1 | 1.70  | 1,235,277 | 3.77  |   286,193 | 286,193 |  509  |   60.3   ||   5.6  |  10.6  | 131.9 |  70.9 |  57.6 | decompose off, edge split on,  2671 oversized leaves, avg  7.0 extra triangles.
 * |          |         |      |  43.9 | 1.46  | 1,057,694 | 3.23  |   249,205 | 249,205 |  688  |   53.3   ||   6.2  |  10.9  | 133.4 |  70.8 |  81.4 | decompose off, edge split off, 4206 oversized leaves, avg 34.0 extra triangles.
 * |          |         |      |       |       |           |       |           |         |       |          ||        |        |       |       |       |
 * |          |         |      |  35.0 | 1.17  |   831,193 | 2.54  |   212,601 | 212,601 |  649  |   54.9   ||   6.3  |  13.3  | 118.7 |  62.0 |  67.5 | r489, 1e-4, same revision as "Ray, 12 tri/leaf" using new laptop
 * |          |         |      |  35.0 | 1.17  |   831,193 | 2.54  |   212,601 | 212,601 |  649  |   54.9   ||   6.1  |  13.3  | 118.7 |  62.0 |  67.5 | r615, 1e-4, right before epsilon change
 * |          |         |      |  38.3 | 1.28  |   908,240 | 2.77  |   233,512 | 233,512 |  495  |   54.4   ||   6.0  |  11.9  | 120.8 |  63.9 |  62.4 | r616, 6e-5, epsilon change
 * |          |         |      |  35.0 | 1.17  |   831,193 | 2.54  |   212,601 | 212,601 |  649  |   54.9   ||   6.2  |  13.3  | 118.7 |  62.0 |  67.5 | r616, 1e-4, confirmation that epsilon is the only contributor
 * |          |         |      |  40.3 | 1.34  |   964,653 | 2.94  |   234,129 | 234,129 |  542  |   61.3   ||   6.1  |  13.1  | 118.8 |  62.8 |  67.6 | r621, 1e-4, crash fix with original epsilon
 * |          |         |      |  50.2 | 1.67  | 1,202,316 | 3.67  |   293,954 | 293,954 |  521  |   61.0   ||   6.0  |  10.5  | 129.7 |  69.6 |  58.0 | r621, 1e-5, crash fix with reduced epsilon
 * |          |         |      |  40.3 | 1.34  |   964,653 | 2.94  |   234,129 | 234,129 |  542  |   61.3   ||   5.7  |  13.1  | 118.8 |  62.8 |  67.6 | r630, 1e-4, performance confirmation on latest revision **
 * |          |         |      |  50.2 | 1.67  | 1,202,316 | 3.67  |   293,954 | 293,954 |  521  |   61.0   ||   5.6  |  10.5  | 129.7 |  69.6 |  58.0 | r630, 1e-5, performance confirmation on latest revision
 */

#if ENABLE_BSP_PROFILING
  #define BSP_PROFILE(x) x
#else
  #define BSP_PROFILE(x)
#endif

const BSPNodeRel BSPNodeRel_Parent = 0;
const BSPNodeRel BSPNodeRel_Back   = 1;
const BSPNodeRel BSPNodeRel_Front  = 2;

static const int32 BackIndex      = 0;
static const int32 FrontIndex     = 1;
static const int32 NullIndex      = 0;
static const int32 RootNodeIndex  = 1;
static const int32 EmptyLeafIndex = 1;

/* --- Running -------------------------------------------------------------- */

#include "Polygon.h"
#include "Ray.h"
#include "Triangle.h"

struct BSPDebug_IntersectionData {
  int32 leaves;
  int32 nodes;
  int32 depth;
  int32 triangles;
  int32 count;
};

struct BSPDebug_Data {
  /* Build Profiling */
  float meshMiB;
  int32 meshTriCount;
  int32 maxDepth;
  int32 nodeCount;
  int32 leafCount;
  float avgTrisPerLeaf;
  float avgLeafDepth;
  float usedMiB;
  float usedMiBRatio;
  int32 triCount;
  float triCountRatio;
  //float maxUsedMiB;
  int32 oversizedNodes;
  float avgOversizeAmount;

  /* Intersection Test Profiling */
  BSPDebug_IntersectionData ray;
  BSPDebug_IntersectionData sphere;
};

struct BSPNode {
  Plane plane;
  BSPNodeRef child[2];
};

struct BSP {
  BSPNodeRef          rootNode;
  BSPNodeRef          emptyLeaf;
  ArrayList(BSPNode,  nodes);
  ArrayList(Triangle, triangles);

  BSP_PROFILE (
    BSPDebug_Data profilingData;
  )
};

struct DelayRay {
  BSPNodeRef nodeRef;
  float      tMin;
  float      tMax;
  int32      depth;
};

struct Delay {
  BSPNodeRef nodeRef;
  int32      depth;
};

ArrayList(DelayRay, rayStack);
bool BSP_IntersectRay (BSP* self, Ray const* _ray, float* tHit) {
  Assert(RAY_INTERSECTION_EPSILON > PLANE_THICKNESS_EPSILON);

  Ray ray = *_ray;
  *tHit = FLT_MAX;

  BSPNodeRef nodeRef  = self->rootNode;
  float      tEpsilon = RAY_INTERSECTION_EPSILON / Vec3f_Length(ray.dir);
  bool       hit      = false;
  int32      depth    = 0;
  int32      maxDepth = 0;

  for (;;) {
    maxDepth = Max(depth, maxDepth);

    if (nodeRef.index >= 0) {
      BSPNode* node = ArrayList_GetPtr(self->nodes, nodeRef.index);
      BSP_PROFILE(self->profilingData.ray.nodes++;)

      float dist = Vec3f_Dot(node->plane.n, ray.p) - node->plane.d;
      float denom = -Vec3f_Dot(node->plane.n, ray.dir);

      /* Near means the side of the plane the point p is on. */
      /* Early means the side of the plane we'll check first. */
      int nearIndex = dist > 0;
      int earlyIndex = nearIndex;

      if (denom != 0.0f) {
        /* Ray not parallel to plane */
        float t = dist / denom;
        float planeBegin = t - tEpsilon;
        float planeEnd   = t + tEpsilon;

        if (planeBegin >= ray.tMax) {
          /* Entire ray lies on the near side */
        }
        else if (planeEnd <= ray.tMin) {
          /* Entire ray lies on one side */
          earlyIndex = (t >= 0.0f) ^ nearIndex;
        }
        else {
          /* Ray touches thick plane */
          earlyIndex = (t < 0.0f) ^ nearIndex;

          /* Don't let the ray 'creep past' tMin/tMax */
          float min = Max(planeBegin, ray.tMin);
          float max = Min(planeEnd  , ray.tMax);

          DelayRay d = {node->child[1 ^ earlyIndex], min, ray.tMax, depth};
          ArrayList_Append(rayStack, d);

          ray.tMax = max;
        }
      }
      else {
        /* Ray parallel to plane. */
        if (Abs(dist) < RAY_INTERSECTION_EPSILON) {
          earlyIndex = nearIndex;

          DelayRay d = {node->child[1 ^ earlyIndex], ray.tMin, ray.tMax, depth};
          ArrayList_Append(rayStack, d);
        }
        else {
          /* Ray outside of thick plane */
        }
      }

      depth++;
      nodeRef = node->child[earlyIndex];
    }

    else {
      Triangle const* leaf = ArrayList_GetPtr(self->triangles, -nodeRef.index);
      BSP_PROFILE(self->profilingData.ray.leaves++;)

      for (uint8 i = 0; i < nodeRef.triangleCount; i++) {
        Triangle const* triangle = leaf + i;
        BSP_PROFILE(self->profilingData.ray.triangles++;)

        float t;
        //if (Intersect_RayTriangle_Barycentric(ray, triangle, tEpsilon, &t)) {
        if (Intersect_RayTriangle_Moller1(&ray, triangle, &t)) {
        //if (Intersect_RayTriangle_Moller2(ray, triangle, &t)) {
        //if (Intersect_RayTriangle_Badouel(ray, triangle, tEpsilon, &t)) {
          if (!hit || t < *tHit) {
            hit = true;
            *tHit = t;
          }
        }
      }

      if (hit) break;

      if (ArrayList_GetSize(rayStack) == 0) break;
      DelayRay d = ArrayList_PopRet(rayStack);
      nodeRef   = d.nodeRef;
      ray.tMin = d.tMin;
      ray.tMax = d.tMax;
      depth     = d.depth;
    }
  }

  ArrayList_Clear(rayStack);
  BSP_PROFILE (
    self->profilingData.ray.count++;
    self->profilingData.ray.depth += maxDepth;
  )

  return hit;
}

bool BSP_IntersectLineSegment (BSP* self, LineSegment const* lineSegment, Vec3f* pHit) {
  float t;
  Vec3f dir = Vec3f_Sub(lineSegment->p1, lineSegment->p0);
  Ray ray = {lineSegment->p0, dir, 0.0f, 1.0f};
  if (BSP_IntersectRay(self, &ray, &t)) {
    Ray_GetPoint(&ray, t, pHit);
    return true;
  }
  return false;
}

ArrayList(Delay, nodeStack);
bool BSP_IntersectSphere (BSP* self, Sphere const* sphere, Vec3f* pHit) {
  Assert(SPHERE_INTERSECTION_EPSILON > PLANE_THICKNESS_EPSILON);

  BSPNodeRef nodeRef  = self->rootNode;
  bool       hit      = false;
  int32      depth    = 0;
  int32      maxDepth = 0;

  for (;;) {
    maxDepth = Max(depth, maxDepth);

    if (nodeRef.index >= 0) {
      BSPNode* node = ArrayList_GetPtr(self->nodes, nodeRef.index);
      BSP_PROFILE(self->profilingData.sphere.nodes++;)

      float dist = Vec3f_Dot(node->plane.n, sphere->p) - node->plane.d;
      if (dist > (sphere->r + SPHERE_INTERSECTION_EPSILON)) {
        /* Entirely in front half-space */
        nodeRef = node->child[FrontIndex];
      }
      else if (dist < -(sphere->r + SPHERE_INTERSECTION_EPSILON)) {
        /* Entirely in back half-space */
        nodeRef = node->child[BackIndex];
      }
      else {
        /* Straddling the thick plane */
        Delay d = { node->child[BackIndex], depth };
        ArrayList_Append(nodeStack, d);
        nodeRef = node->child[FrontIndex];
      }

      depth++;
    }
    else {
      Triangle* leaf = ArrayList_GetPtr(self->triangles, -nodeRef.index);
      BSP_PROFILE(self->profilingData.sphere.leaves++;)

      for (uint8 i = 0; i < nodeRef.triangleCount; i++) {
        Triangle* triangle = leaf + i;
        BSP_PROFILE(self->profilingData.sphere.triangles++;)

        Vec3f pHit2;
        if (Intersect_SphereTriangle(sphere, triangle, &pHit2)) {
          hit = true;
          *pHit = pHit2;
          break;
        }
      }
      if (hit) break;

      if (ArrayList_GetSize(nodeStack) == 0) break;
      Delay d = ArrayList_PopRet(nodeStack);
      nodeRef = d.nodeRef;
      depth   = d.depth;
    }
  }

  ArrayList_Clear(nodeStack);
  BSP_PROFILE (
    self->profilingData.sphere.count++;
    self->profilingData.sphere.depth += maxDepth;
  )

  return hit;
}

/* --- Building ------------------------------------------------------------- */

#include <float.h>
#include "Mesh.h"
#include "RNG.h"
#include "Vertex.h"

#define DEFAULT_TRIANGLE_SPLIT_COST 0.85f
#define LEAF_TRIANGLE_COUNT 12
#define MAX_LEAF_TRIANGLE_COUNT (1 << 8*sizeof(BSPNodeRef::triangleCount))

typedef uint8 PolygonFlag;
const PolygonFlag PolygonFlag_None             = 0 << 0;
const PolygonFlag PolygonFlag_InvalidFaceSplit = 1 << 0;
const PolygonFlag PolygonFlag_InvalidDecompose = 1 << 1;
const PolygonFlag PolygonFlag_InvalidEdgeSplit = 1 << 2;

struct PolygonEx {
  ArrayList(Vec3f, vertices);
  PolygonFlag flags;
};

struct BSPBuild_Node {
  Plane plane;
  BSPBuild_Node* child[2];
  ArrayList(PolygonEx, polygons);

  CHECK2 (
    int32 id;
    BSPBuild_Node* parent;
    Vec3f planeCenter;
  )
};

struct BSPBuild {
  BSPBuild_Node* rootNode;
  RNG* rng;
  int32 nodeCount;
  int32 leafCount;
  int32 triangleCount;

  CHECK2 (
    int32 nextNodeID;
    int32 oversizedNodes;
    float avgOversizeAmount;
  )
};

struct BSPBuild_NodeData {
  ArrayList(PolygonEx, polygons);
  int32 validPolygonCount;
  int32 triangleCount;
  uint16 depth;
  //Box3f boundingBox;
  //uint8 cutIndex;
};

static float BSPBuild_ScoreSplitPlane (BSPBuild_NodeData* nodeData, Plane plane, float k) {
  /* The bigger k is, the more we penalize polygon splitting */
  Assert(k >= 0.0f && k <= 1.0f);

  int32 numInFront    = 0;
  int32 numBehind     = 0;
  int32 numStraddling = 0;

  ArrayList_ForEach(nodeData->polygons, PolygonEx, polygon) {
    PolygonClassification classification = Plane_ClassifyPolygon(&plane, (Polygon*) polygon);
    switch (classification) {
      default: Fatal("BSPBuild_ScoreSplitPlane: Unhandled case: %i", classification);

      case PolygonClassification_Coplanar:
      case PolygonClassification_Behind:     numBehind++;     break;
      case PolygonClassification_InFront:    numInFront++;    break;
      case PolygonClassification_Straddling: numStraddling++; break;

    }
  }

  //k*numStraddling + (1.0f - k)*Abs(numInFront - numBehind);
  float score = Lerp((float) Abs(numInFront - numBehind), (float) numStraddling, k);
  return score;
}

static bool BSPBuild_ChooseSplitPlane (
  BSPBuild* bsp,
  CHECK2PARAM(BSPBuild_Node* node)
  BSPBuild_NodeData* nodeData,
  Plane* splitPlane)
{
  /* See Realtime Collision Detection pp361-363 */

  /* Misc Notes from the Literature
   *  TODO : The number of candidates c selected at each call as a percentage of the
   *  number of faces f lying in the current region is increased as a linear function
   *  of f until a predetermined threshold is reached, after which all face hyperplanes
   *  are chosen (currently 20).
   *
   *  NOTE: Since we are interested in generating a multiresolution representation,
   *  we bias the selection process by first sorting the face hyperplanes by area
   *  (each hyperplane is represented only once, and has with it a list of coincident
   *  faces). The candidates are then the first c on this sorted list.
   *  https://pdfs.semanticscholar.org/8fa2/b73cb14fad3abe749a0da4fba50f18a19e2a.pdf
   *  This method sucked! Vastly slower than random choices. I only sorted the list
   *  once, not every single split. Perhaps this breaks the algorithm? Either way,
   *  sorting triangles every single split during tree construction is going to
   *  annihilate build time.
   *
   *  TODO : For each of a predefined number of directions, we project all of the
   *  vertices onto that direction and then sort them. We then consider hyperplanes
   *  orthogonal to this direction which contain vertices at certain positions in the
   *  ordering. The percentage of positions tested is treated similarly to that for
   *  choosing the number of face hyperplanes. The directions we are currently using
   *  correspond to the k-faces of a hypercube, whose number in 3D is 13 = 26/2 (see
   *  figure 7 for the 2D case).
   *  https://pdfs.semanticscholar.org/8fa2/b73cb14fad3abe749a0da4fba50f18a19e2a.pdf
   *
   *  TODO : The third method is similar to the second, but uses least squares fit
   *  to generate a direction. In particular, we compute the least squares fit of the
   *  set of vertices lying in the current region, and then use the normal of the
   *  resulting hyperplane as a new direction when applying the same techniques as
   *  used with the predefined directions.
   *  https://pdfs.semanticscholar.org/8fa2/b73cb14fad3abe749a0da4fba50f18a19e2a.pdf
   */

  float maxDepth = 1000.0f;
  float biasedDepth = (float) nodeData->depth - 100.0f;
  float t = Max(biasedDepth / maxDepth, 0.0f);
  float k = Lerp(DEFAULT_TRIANGLE_SPLIT_COST, 0.25f, t);

  float      bestScore   = FLT_MAX;
  Plane      bestPlane   = {};
  PolygonEx* bestPolygon = 0;
  int32      numToCheck  = 10;

  int32 polygonsLen = ArrayList_GetSize(nodeData->polygons);

  if (nodeData->validPolygonCount > 0) {
    /* Simply score split planes using polygon faces */
    numToCheck = Min(numToCheck, nodeData->validPolygonCount);
    for (int32 i = 0; i < numToCheck; i++) {
      int32 polygonIndex = RNG_Get32(bsp->rng) % polygonsLen;

      /* OPTIMIZE: This search is duuuuuumb. Maybe We should swap invalid
       *           polygons to the end of the list so never have to search.
       */
      for (int32 j = 0; j < polygonsLen; j++) {
        PolygonEx* polygon = ArrayList_GetPtr(nodeData->polygons, polygonIndex);

        if (!(polygon->flags & PolygonFlag_InvalidFaceSplit)) {
          Plane plane; Polygon_ToPlane((Polygon*) polygon, &plane);
          float score = BSPBuild_ScoreSplitPlane(nodeData, plane, k);

          if (score < bestScore) {
            bestScore   = score;
            bestPlane   = plane;
            bestPolygon = polygon;
          }
          break;
        }

        polygonIndex = (polygonIndex + 1) % polygonsLen;
      }
    }

    if (bestPolygon) {
      bestPolygon->flags |= PolygonFlag_InvalidFaceSplit;
      CHECK2(Polygon_GetCentroid((Polygon*) bestPolygon, &node->planeCenter);)
    }
  }
  else if (polygonsLen > 0) {
    /* No remaining polygons are valid for splitting. So we split any polygons
     * with multiple triangles. When none of those are left, we use the polygon
     * edges as split planes with no penalty for cutting other polygons.
     */

    /* EDGE: It's possible to get to a point where all remaining polygons are
     * invalid for auto partitioning, but there are still more triangles than
     * the max leaf size. In this case we need to start dividing the polygons.
     * If we don't do this, it makes a significant impact on overall tree size
     * because we actually end up with quite a few leaves with more triangles
     * than MAX_LEAF_TRIANGLE_COUNT
     */

    /* EDGE: With very few polygons BSPBuild_ScoreSplitPlane will prioritize 100%
     * lopsided splits over a split with a single cut. This leads to picking
     * the same, useless general cut again next time.
     */

    /* Note that the flags set by these additional splitting steps will be
     * transferred to the resulting pieces if the polygon is ever split. This
     * is currently necessary because if the cut is chosen but this polygon
     * can't be split safely (produces degenerate or tiny polgons, see
     * Polygon_SplitSafe) we create 2 new 'split' polygons that are actually
     * the full polygon and send it to both sides of the plane. We might be
     * able to remove this for slightly better splitting (e.g. ending up with
     * fewer oversized leaves because we tried more cuts) but it needs to be
     * done carefully. */

    bool splitFound = false;

    /* Try to split any polygons with more than 1 triangle */
    if (!splitFound) {
      int32 polygonIndex = RNG_Get32(bsp->rng) % polygonsLen;
      for (int32 i = 0; i < polygonsLen; i++) {
        PolygonEx* polygon = ArrayList_GetPtr(nodeData->polygons, polygonIndex);
        if (polygon->flags & PolygonFlag_InvalidDecompose) continue;

        Vec3f* v    = ArrayList_GetData(polygon->vertices);
        int32  vLen = ArrayList_GetSize(polygon->vertices);
        for (int32 j = 2; j < vLen - 1; j++) {
          Vec3f edge = Vec3f_Sub(v[0], v[j]);
          Vec3f mid  = Vec3f_Lerp(v[0], v[j], 0.5f);

          /* TODO : Maybe just save the plane with polygon while build so they're only calculated once? */
          Plane polygonPlane;
          Polygon_ToPlane((Polygon*) polygon, &polygonPlane);

          Plane plane;
          plane.n = Vec3f_Normalize(Vec3f_Cross(edge, polygonPlane.n));
          plane.d = Vec3f_Dot(plane.n, mid);

          /* TODO : Proper scoring? */
          if (Plane_ClassifyPolygon(&plane, (Polygon*) polygon) == PolygonClassification_Straddling) {
            splitFound = true;

            bestScore   = 0;
            bestPlane   = plane;
            bestPolygon = polygon;
            CHECK2(node->planeCenter = mid;)
            break;
          }
          else {
            /* This is possible because we don't fully handle slivers. There's
             * nothing stopping a triangle from being thinner than
             * PLANE_THICKNESS_EPSILON. */
            polygon->flags |= PolygonFlag_InvalidDecompose;
          }
          //if (--numToCheck == 0) break;
        }

        if (splitFound) break;
        //if (numToCheck == 0) break;
        polygonIndex = (polygonIndex + 1) % polygonsLen;
      }

      if (splitFound)
        bestPolygon->flags |= PolygonFlag_InvalidDecompose;
    }

    /* Try splitting along a polygon edge */
    if (!splitFound) {
      int32 polygonIndex = RNG_Get32(bsp->rng) % polygonsLen;
      for (int32 i = 0; i < polygonsLen; i++) {
        PolygonEx* polygon = ArrayList_GetPtr(nodeData->polygons, polygonIndex);
        if (polygon->flags & PolygonFlag_InvalidEdgeSplit) continue;

        Plane polygonPlane;
        Polygon_ToPlane((Polygon*) polygon, &polygonPlane);

        Vec3f* v    = ArrayList_GetData(polygon->vertices);
        int32  vLen = ArrayList_GetSize(polygon->vertices);

        Vec3f  vPrev = v[vLen - 1];
        for (int32 j = 0; j < vLen; j++) {
          Vec3f vCur = v[j];
          Vec3f edge = Vec3f_Sub(vCur, vPrev);
          Vec3f mid  = Vec3f_Lerp(vPrev, vCur, 0.5f);

          Plane plane;
          plane.n = Vec3f_Normalize(Vec3f_Cross(edge, polygonPlane.n));
          plane.d = Vec3f_Dot(plane.n, mid);

          float score = BSPBuild_ScoreSplitPlane(nodeData, plane, 0);
          if (score < bestScore) {
            splitFound = true;

            bestPolygon = polygon;
            bestScore   = score;
            bestPlane   = plane;
            CHECK2(node->planeCenter = mid;)
          }

          vPrev = vCur;
          if (--numToCheck == 0) break;
        }

        if (numToCheck == 0) break;
        polygonIndex = (polygonIndex + 1) % polygonsLen;
      }

      if (splitFound)
        bestPolygon->flags |= PolygonFlag_InvalidEdgeSplit;
    }

    CHECK3 (
      /* Still nothing. Fuck it. */
      if (!splitFound) {
        int32 triangleCount = 0;
        ArrayList_ForEach(nodeData->polygons, PolygonEx, polygon) {
          triangleCount += ArrayList_GetSize(polygon->vertices) - 2;
        }
        bsp->oversizedNodes++;
        float oversizeAmount = (float) (triangleCount - LEAF_TRIANGLE_COUNT);
        bsp->avgOversizeAmount = Lerp(bsp->avgOversizeAmount, oversizeAmount, 1.0f / bsp->oversizedNodes);
        Warn("BSPBuild_ChooseSplitPlane: Failed to find a good split. Giving up. Leaf will have %i triangles.", triangleCount);
      }
    )
  }

  else {
    /* We don't have any polygons left. All of them were on the same side of
     * the last split. This will end up being a leaf.  */
  }

  if (bestScore < FLT_MAX) {
    *splitPlane = bestPlane;
    return true;
  }
  else {
    return false;
  }
}

inline static void BSPBuild_AppendPolygon (BSPBuild_NodeData* nodeData, PolygonEx* polygon) {
  //if (nodeData->triangleCount == 0) {
  //  Vec3f v0 = ArrayList_Get(polygon->vertices, 0);
  //  nodeData->boundingBox.lower = v0;
  //  nodeData->boundingBox.upper = v0;
  //}
  //ArrayList_ForEach(polygon->vertices, Vec3f, v) {
  //  Box3f_Add(&nodeData->boundingBox, *v);
  //}

  nodeData->triangleCount += ArrayList_GetSize(polygon->vertices) - 2;
  nodeData->validPolygonCount += (int) !(polygon->flags & PolygonFlag_InvalidFaceSplit);
  ArrayList_Append(nodeData->polygons, *polygon);
}

static BSPBuild_Node* BSPBuild_CreateNode (BSPBuild* bsp, BSPBuild_NodeData* nodeData) {
  /* NOTE: This will free the polygons being passed in! This is to prevent all
   *        the temporary allocations from overlapping. */

  /* NOTE: Coplanar polygons are considered to be behind the plane and will
   *        therefore lead to collisions. It seems preferable to push objects
   *        very slightly outside of each other during a collision, rather than
   *        letting them very slightly overlap. */

  Assert(nodeData->depth < 1 << 8*sizeof(nodeData->depth));

  BSPBuild_Node* node = MemNewZero(BSPBuild_Node);
  CHECK2(node->id = bsp->nextNodeID++;)

  Plane splitPlane = {};

  bool makeLeaf = false;
  makeLeaf = makeLeaf || nodeData->triangleCount <= LEAF_TRIANGLE_COUNT;
  makeLeaf = makeLeaf || !BSPBuild_ChooseSplitPlane(bsp, CHECK2PARAM(node) nodeData, &splitPlane);

  if (makeLeaf) {
    if (nodeData->triangleCount != 0)
      bsp->leafCount++;
    bsp->triangleCount += nodeData->triangleCount;

    node->polygons_capacity = nodeData->polygons_capacity;
    node->polygons_size     = nodeData->polygons_size;
    node->polygons_data     = nodeData->polygons_data;
    return node;
  }

  bsp->nodeCount++;

  int32 polygonsLen = ArrayList_GetSize(nodeData->polygons);

  BSPBuild_NodeData backNodeData = {};
  ArrayList_Reserve(backNodeData.polygons, polygonsLen);
  backNodeData.depth = nodeData->depth + 1;

  BSPBuild_NodeData frontNodeData = {};
  ArrayList_Reserve(frontNodeData.polygons, polygonsLen);
  frontNodeData.depth = nodeData->depth + 1;

  ArrayList_ForEach(nodeData->polygons, PolygonEx, polygon) {
    PolygonClassification classification = Plane_ClassifyPolygon(&splitPlane, (Polygon*) polygon);
    switch (classification) {
      default: Fatal("BSPBuild_CreateNode: Unhandled case: %i", classification);

      case PolygonClassification_Coplanar:
        polygon->flags |= PolygonFlag_InvalidFaceSplit;
        /* Fall through */

      case PolygonClassification_Behind:
        BSPBuild_AppendPolygon(&backNodeData, polygon);
        break;

      case PolygonClassification_InFront:
        BSPBuild_AppendPolygon(&frontNodeData, polygon);
        break;

      case PolygonClassification_Straddling: {
        PolygonEx backPart = {};   backPart.flags = polygon->flags;
        PolygonEx frontPart = {}; frontPart.flags = polygon->flags;

        Polygon_SplitSafe((Polygon*) polygon, splitPlane, (Polygon*) &backPart, (Polygon*) &frontPart);
        BSPBuild_AppendPolygon(&backNodeData, &backPart);
        BSPBuild_AppendPolygon(&frontNodeData, &frontPart);

        ArrayList_Free(polygon->vertices);
        break;
      }
    }
  }
  ArrayList_Free(nodeData->polygons);

  node->plane = splitPlane;
  node->child[BackIndex]  = BSPBuild_CreateNode(bsp, &backNodeData);
  node->child[FrontIndex] = BSPBuild_CreateNode(bsp, &frontNodeData);

  CHECK2 (
    node->child[BackIndex] ->parent = node;
    node->child[FrontIndex]->parent = node;
  )
  return node;
}

static BSPNodeRef BSPBuild_OptimizeTree (BSP* self, BSPBuild_Node* buildNode) {
  if (buildNode->child[BackIndex] || buildNode->child[FrontIndex]) {
    /* Node */
    Assert(ArrayList_GetSize(self->nodes) < ArrayList_GetCapacity(self->nodes));

    BSPNode dummy = {};
    int32 nodeIndex = ArrayList_GetSize(self->nodes);
    ArrayList_Append(self->nodes, dummy);
    BSPNode* node = ArrayList_GetLastPtr(self->nodes);

    node->plane = buildNode->plane;
    node->child[BackIndex]  = BSPBuild_OptimizeTree(self, buildNode->child[BackIndex]);
    node->child[FrontIndex] = BSPBuild_OptimizeTree(self, buildNode->child[FrontIndex]);

    BSPNodeRef result = { nodeIndex, 0 };
    return result;
  } else {
    /* Leaf */
    if (ArrayList_GetSize(buildNode->polygons) == 0)
      return self->emptyLeaf;

    int32 leafIndex = ArrayList_GetSize(self->triangles);

    ArrayList_ForEach(buildNode->polygons, PolygonEx, polygon) {
      Assert(
        ArrayList_GetSize(self->triangles) +
        ArrayList_GetSize(polygon->vertices) - 2
        <= ArrayList_GetCapacity(self->triangles)
      );

      Polygon_ConvexToTriangles((Polygon*) polygon,
        &ArrayList_GetCapacity(self->triangles),
        &ArrayList_GetSize(self->triangles),
        &ArrayList_GetData(self->triangles)
     );
    }

    uint8 leafLen = (uint8) (ArrayList_GetSize(self->triangles) - leafIndex);
    BSPNodeRef result = { -leafIndex, leafLen };
    return result;
  }
}

static void BSPBuild_FreeNode (BSPBuild_Node* node) {
  if (node->child[BackIndex] || node->child[FrontIndex]) {
    BSPBuild_FreeNode(node->child[BackIndex]);
    BSPBuild_FreeNode(node->child[FrontIndex]);
  }
  else {
    ArrayList_ForEach(node->polygons, PolygonEx, polygon) {
      ArrayList_Free(polygon->vertices);
    }
    ArrayList_Free(node->polygons);
  }
  MemFree(node);
}

BSP_PROFILE (
static void BSPBuild_AnalyzeTree (BSP* self, Mesh* mesh, BSPNodeRef nodeRef, int32 depth) {
  BSPDebug_Data* pd = &self->profilingData;

  /* TODO : Do this while building */

  /* All */
  pd->maxDepth = Max(pd->maxDepth, depth);

  /* Internal */
  if (nodeRef.index >= 0) {
    BSPNode* node = ArrayList_GetPtr(self->nodes, nodeRef.index);
    BSPBuild_AnalyzeTree(self, mesh, node->child[BackIndex] , depth + 1);
    BSPBuild_AnalyzeTree(self, mesh, node->child[FrontIndex], depth + 1);
  }
  /* Leaf */
  else {
    pd->leafCount++;
    pd->avgLeafDepth = Lerp(pd->avgLeafDepth, (float) depth, 1.0f / (float) pd->leafCount);
  }

  /* Root */
  if (depth == 0) {
    const float BToMiB = 1.0f / 1024.0f / 1024.0f;

    pd->nodeCount = ArrayList_GetCapacity(self->nodes);
    pd->usedMiB += pd->nodeCount * sizeof(BSPNode);

    pd->triCount += ArrayList_GetCapacity(self->triangles);
    pd->usedMiB += pd->triCount * sizeof(Triangle);

    pd->meshTriCount = Mesh_GetIndexCount(mesh) / 3;
    pd->meshMiB = (float) Mesh_GetIndexCount(mesh) * sizeof(int32);
    pd->meshMiB = (float) Mesh_GetVertexCount(mesh) * sizeof(Vertex);
    pd->meshMiB *= BToMiB;

    pd->usedMiB += sizeof(BSP);
    pd->usedMiB *= BToMiB;

    pd->triCountRatio = (float) pd->triCount / (float) pd->meshTriCount;
    pd->usedMiBRatio = (float) pd->usedMiB / (float) pd->meshMiB;
  }
}
)

BSP* BSP_Create (Mesh* mesh) {
  Assert(LEAF_TRIANGLE_COUNT <= MAX_LEAF_TRIANGLE_COUNT);

  /* NOTE: This function will use memory proportional to 2x the mesh memory.
   *        There will be one copy of all the polygons & vertices in the initial
   *        list of polygons passed to BSPBuild_CreateNode, which will then create new
   *        lists of polygons for the back and front, but will reuse the vertices
   *        from the original list. Therefore we never have more vertices than we
   *        actually need during tree building (aside from the fact that polygons
   *        don't share vertices), but we do have 2 copys of each polygon
   *        temporarily. Then during BSPBuild_OptimizeTree, all the resulting polygons
   *        will be decomposed into triangles which will temporarily store 2
   *        copies of all vertices. The 2x figure is slightly hand-wavy because
   *        splitting will increase the total number of vertices, but I'm
   *        assuming that doesn't get too out of hand for now. Since the mesh
   *        stores indices and vertex attributes I expect the proportionality
   *        constant to be in the ballpark of 0.5 */

  BSP* self = MemNewZero(BSP);

  int32   indexLen   = Mesh_GetIndexCount(mesh);
  int32*  indexData  = Mesh_GetIndexData(mesh);
  Vertex* vertexData = Mesh_GetVertexData(mesh);

  /* TODO : Implement some form of soft abort when the incoming mesh is bad. */
  CHECK2 (
    if (Mesh_Validate(mesh) != Error_None) return 0;
  )

  BSPBuild_NodeData nodeData = {};
  nodeData.triangleCount     = indexLen / 3;
  nodeData.validPolygonCount = indexLen / 3;

  ArrayList_Reserve(nodeData.polygons, nodeData.triangleCount);
  for (int32 i = 0; i < indexLen; i += 3) {
    int32 i0 = indexData[i + 0];
    int32 i1 = indexData[i + 1];
    int32 i2 = indexData[i + 2];
    Vec3f v0 = vertexData[i0].p;
    Vec3f v1 = vertexData[i1].p;
    Vec3f v2 = vertexData[i2].p;

    PolygonEx polygon = {};
    ArrayList_Reserve(polygon.vertices, 3);
    ArrayList_Append(polygon.vertices, v0);
    ArrayList_Append(polygon.vertices, v1);
    ArrayList_Append(polygon.vertices, v2);

    ArrayList_Append(nodeData.polygons, polygon);
  }

  /* Build */
  BSPBuild bspBuild = {};
  bspBuild.rng      = RNG_Create(1235);
  bspBuild.rootNode = BSPBuild_CreateNode(&bspBuild, &nodeData);

  /* Optimize */
  Triangle nullLeaf = {};
  ArrayList_Reserve(self->triangles, bspBuild.triangleCount + 2);
  ArrayList_Append(self->triangles, nullLeaf); /* Null leaf */
  ArrayList_Append(self->triangles, nullLeaf); /* Empty leaf */
  self->emptyLeaf.index = -EmptyLeafIndex;
  self->emptyLeaf.triangleCount = 0;

  BSPNode nullNode = {};
  ArrayList_Reserve(self->nodes, bspBuild.nodeCount + 1);
  ArrayList_Append(self->nodes, nullNode); /* Null node */
  self->rootNode = BSPBuild_OptimizeTree(self, bspBuild.rootNode);
  #if BSP_PROFILE && CHECK_LEVEL >= 2
    self->profilingData.oversizedNodes = bspBuild.oversizedNodes;
    self->profilingData.avgOversizeAmount = bspBuild.avgOversizeAmount;
    if (bspBuild.oversizedNodes > 0) {
      Warn("BSP_Create: Created %i oversized leaves with an average excess of %.1f triangles.", bspBuild.oversizedNodes, bspBuild.avgOversizeAmount);
    }
  #endif

  BSPBuild_FreeNode(bspBuild.rootNode);
  RNG_Free(bspBuild.rng);

  Assert(ArrayList_GetSize(self->nodes)     == ArrayList_GetCapacity(self->nodes));
  Assert(ArrayList_GetSize(self->triangles) == ArrayList_GetCapacity(self->triangles));
  BSP_PROFILE(BSPBuild_AnalyzeTree(self, mesh, self->rootNode, 0);)

  return self;
}

void BSP_Free (BSP* self) {
  if (!self)
    return;

  ArrayList_Free(self->nodes);
  ArrayList_Free(self->triangles);

  MemFree(self);
}

/* --- Debuging ------------------------------------------------------------- */

#include "BlendMode.h"
#include "CullFace.h"
#include "Draw.h"
#include "RenderState.h"

BSPNodeRef BSPDebug_GetNode (BSP* self, BSPNodeRef nodeRef, BSPNodeRel relationship) {
  if (!self)
    Fatal("BSP_GetNode: bsp is null");

  if (!nodeRef.index)
    return self->rootNode;

  BSPNode* node = 0;
  if (nodeRef.index > 0)
    node = ArrayList_GetPtr(self->nodes, nodeRef.index);

  BSPNodeRef newNode = {};
  switch (relationship) {
    default: Fatal("BSPDebug_GetNode: Unhandled case: %i", relationship);

    case BSPNodeRel_Parent:
      if (nodeRef.index) {
        for (int32 i = 0; i < ArrayList_GetSize(self->nodes); i++) {
          BSPNode* nodeToCheck = ArrayList_GetPtr(self->nodes, i);

          if (nodeToCheck->child[BackIndex].index == nodeRef.index) {
            newNode.index = i;
            break;
          }

          if (nodeToCheck->child[FrontIndex].index == nodeRef.index) {
            newNode.index = i;
            break;
          }
        }
      }
      break;

    case BSPNodeRel_Back:
      if (node) newNode = node->child[BackIndex];
      break;

    case BSPNodeRel_Front:
      if (node) newNode = node->child[FrontIndex];
      break;
  }

  return newNode.index ? newNode : nodeRef;
}

void BSPDebug_DrawNode (BSP* self, BSPNodeRef nodeRef) {
  Assert(nodeRef.index);

  if (nodeRef.index > 0) {
    BSPNode* node = ArrayList_GetPtr(self->nodes, nodeRef.index);
    BSPDebug_DrawNode(self, node->child[BackIndex]);
    BSPDebug_DrawNode(self, node->child[FrontIndex]);
  }
  else {
    Triangle* leaf = ArrayList_GetPtr(self->triangles, -nodeRef.index);
    for (uint8 i = 0; i < nodeRef.triangleCount; i++) {
      Triangle* triangle = leaf + i;
      Draw_Poly3(triangle->vertices, 3);
    }
  }
}

void BSPDebug_DrawNodeSplit (BSP* self, BSPNodeRef nodeRef) {
  Assert(nodeRef.index);

  RenderState_PushBlendMode(BlendMode_Alpha);
  RenderState_PushCullFace(CullFace_Back);
  RenderState_PushDepthTest(true);
  RenderState_PushWireframe(true);

  if (nodeRef.index > 0) {
    BSPNode* node = ArrayList_GetPtr(self->nodes, nodeRef.index);

    /* Back */
    Draw_Color(0.5f, 0.3f, 0.3f, 0.4f);
    BSPDebug_DrawNode(self, node->child[BackIndex]);

    /* Front */
    Draw_Color(0.3f, 0.5f, 0.3f, 0.4f);
    BSPDebug_DrawNode(self, node->child[FrontIndex]);

    /* Plane */
    Vec3f closestPoint;
    {
      Vec3f origin = {};
      float t = Vec3f_Dot(node->plane.n, origin) - node->plane.d;
      closestPoint = Vec3f_Sub(origin, Vec3f_Muls(node->plane.n, t));
    }
    RenderState_PushWireframe(false);
    Draw_Color(0.3f, 0.5f, 0.3f, 0.4f);
    Draw_Plane(&closestPoint, &node->plane.n, 2);
    Draw_Color(0.5f, 0.3f, 0.3f, 0.4f);
    Vec3f neg = Vec3f_Muls(node->plane.n, -1.0f);
    Draw_Plane(&closestPoint, &neg, 2);
    RenderState_PopWireframe();
  }
  else {
    /* Leaf */
    Draw_Color(0.5f, 0.5f, 0.3f, 0.4f);
    BSPDebug_DrawNode(self, nodeRef);
  }

  RenderState_PopWireframe();
  RenderState_PopDepthTest();
  RenderState_PopCullFace();
  RenderState_PopBlendMode();
}

void BSPDebug_DrawLineSegment (BSP* bsp, LineSegment* lineSegment) {
  Vec3f pHit;
  if (BSP_IntersectLineSegment(bsp, lineSegment, &pHit)) {
    Draw_Color(0.0f, 1.0f, 0.0f, 0.1f);
    Draw_Line3(&lineSegment->p0, &pHit);

    Draw_Color(1.0f, 0.0f, 0.0f, 1.0f);
    Draw_Line3(&pHit, &lineSegment->p1);

    Draw_PointSize(5.0f);
    Draw_Point3(pHit.x, pHit.y, pHit.z);
  }
  else {
    Draw_Color(0.0f, 1.0f, 0.0f, 1.0f);
    Draw_Line3(&lineSegment->p0, &lineSegment->p1);
  }
}

void BSPDebug_DrawSphere (BSP* self, Sphere* sphere) {
  Vec3f pHit;
  if (BSP_IntersectSphere(self, sphere, &pHit)) {
    RenderState_PushWireframe(false);
    Draw_Color(1.0f, 0.0f, 0.0f, 0.3f);
    Draw_Sphere(&sphere->p, sphere->r);
    RenderState_PopWireframe();

    Draw_Color(1.0f, 0.0f, 0.0f, 1.0f);
    Draw_Sphere(&sphere->p, sphere->r);

    RenderState_PushDepthTest(false);
    Draw_PointSize(8.0f);
    Draw_Point3(UNPACK3(pHit));
    RenderState_PopDepthTest();
  }
  else {
    RenderState_PushWireframe(false);
    Draw_Color(0.0f, 1.0f, 0.0f, 0.3f);
    Draw_Sphere(&sphere->p, sphere->r);
    RenderState_PopWireframe();

    Draw_Color(0.0f, 1.0f, 0.0f, 1.0f);
    Draw_Sphere(&sphere->p, sphere->r);
  }
}

static void BSPDebug_PrintProfilingData (BSP* self, BSPDebug_IntersectionData* data, double totalTime) {
  #if ENABLE_BSP_PROFILING
    BSPDebug_Data* pd = &self->profilingData;

    float us = (float) (totalTime * 1000.0 * 1000.0);
    float avgus     = (float) us              / data->count;
    float avgLeaves = (float) data->leaves    / data->count;
    float avgNodes  = (float) data->nodes     / data->count;
    float avgTris   = (float) data->triangles / data->count;
    float avgDepth  = (float) data->depth     / data->count;

    char buffer[256];
    /*                                            name       tris      mb    bsp mb   mbr    tris   trir     nod   lv    maxd      lvd        ray us    rayl     rayn    rayd    rayt */
    snprintf(buffer, (size_t) Array_GetSize(buffer), "* |          |         |      | %5.1f | %4.2f  | %9d | %4.2f  | %9d | %7d |  %3d  |  %5.1f   ||  %4.1f  |  %4.1f  | %5.1f | %5.1f | %5.1f |\n",
      pd->usedMiB, pd->usedMiBRatio, pd->triCount, pd->triCountRatio,
      pd->nodeCount, pd->leafCount, pd->maxDepth, pd->avgLeafDepth,
      avgus, avgLeaves, avgNodes, avgDepth, avgTris
    );
    puts(buffer);
  #else
    Warn("BSP_PrintProfilingData: BSP profiling is not enabled. Set ENABLE_BSP_PROFILING to enable this function.");
    UNUSED(self); UNUSED(data); UNUSED(totalTime);
    UNUSED(&BSPDebug_PrintProfilingData);
  #endif
}

void BSPDebug_PrintRayProfilingData (BSP* self, double totalTime) {
  #if ENABLE_BSP_PROFILING
    BSPDebug_PrintProfilingData(self, &self->profilingData.ray, totalTime);
  #else
    Warn("BSP_PrintRayProfilingData: BSP profiling is not enabled. Set ENABLE_BSP_PROFILING to enable this function.");
    UNUSED(self); UNUSED(totalTime);
  #endif
}

void BSPDebug_PrintSphereProfilingData (BSP* self, double totalTime) {
  #if ENABLE_BSP_PROFILING
    BSPDebug_PrintProfilingData(self, &self->profilingData.sphere, totalTime);
  #else
    Warn("BSP_PrintSphereProfilingData: BSP profiling is not enabled. Set ENABLE_BSP_PROFILING to enable this function.");
    UNUSED(self); UNUSED(totalTime);
  #endif
}

bool BSPDebug_GetIntersectSphereTriangles (BSP* self, Sphere* sphere, IntersectSphereProfiling* sphereProf) {
  Assert(SPHERE_INTERSECTION_EPSILON > PLANE_THICKNESS_EPSILON);

  BSPNodeRef nodeRef   = self->rootNode;
  bool       hit       = false;
  int32      depth     = 0;
  int32      maxDepth  = 0;

  for (;;) {
    maxDepth = Max(depth, maxDepth);

    if (nodeRef.index >= 0) {
      BSPNode* node = ArrayList_GetPtr(self->nodes, nodeRef.index);
      sphereProf->nodes++;

      float dist = Vec3f_Dot(node->plane.n, sphere->p) - node->plane.d;
      if (dist > (sphere->r + SPHERE_INTERSECTION_EPSILON)) {
        /* Entirely in front half-space */
        nodeRef = node->child[FrontIndex];
      }
      else if (dist < - (sphere->r + SPHERE_INTERSECTION_EPSILON)) {
        /* Entirely in back half-space */
        nodeRef = node->child[BackIndex];
      }
      else {
        /* Straddling the thick plane */
        Delay d = { node->child[BackIndex], depth };
        ArrayList_Append(nodeStack, d);
        nodeRef = node->child[FrontIndex];
      }

      depth++;
    }
    else {
      Triangle* leaf = ArrayList_GetPtr(self->triangles, -nodeRef.index);
      sphereProf->leaves++;

      for (uint8 i = 0; i < nodeRef.triangleCount; i++) {
        Triangle* triangle = leaf + i;
        sphereProf->triangles++;

        Vec3f pHit2;
        if (Intersect_SphereTriangle(sphere, triangle, &pHit2)) {
          TriangleTest t = { triangle, true };
          ArrayList_Append(sphereProf->triangleTests, t);
          hit = true;
          break;
        }
        TriangleTest t = { triangle, false };
        ArrayList_Append(sphereProf->triangleTests, t);
      }
      if (hit) break;

      if (ArrayList_GetSize(nodeStack) == 0) break;
      Delay d = ArrayList_PopRet(nodeStack);
      nodeRef = d.nodeRef;
      depth   = d.depth;
    }
  }

  ArrayList_Clear(nodeStack);

  return hit;
}

BSPNodeRef BSPDebug_GetLeaf (BSP* self, int32 leafIndex) {
  int32 index = -1;
  ArrayList_ForEach(self->nodes, BSPNode, node) {
    if (node->child[0].index < 0)
      if (index++ == leafIndex)
        return node->child[0];

    if (node->child[1].index < 0)
      if (index++ == leafIndex)
        return node->child[1];
  }

  BSPNodeRef result = { RootNodeIndex, 0 };
  return result;
}

/* -------------------------------------------------------------------------- */
