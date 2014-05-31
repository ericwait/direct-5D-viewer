#pragma once

#ifndef SQR
#define SQR(x) ((x)*(x))
#endif

#ifndef MAX
#define MAX(x,y) ((x>y)?(x):(y))
#endif

#ifndef MIN
#define MIN(x,y) ((x<y)?(x):(y))
#endif

#ifndef MAT_TO_C
#define MAT_TO_C(x) (x-1)
#endif

#ifndef C_TO_MAT
#define C_TO_MAT(x) (x+1)
#endif

#ifndef ARRAY_SIZE
#define ARRAY_SIZE(x) (sizeof(x)/sizeof((x)[0]))
#endif

#ifndef SAFE_DELETE
#define SAFE_DELETE(p) { if (p) { delete (p); (p)=NULL; } }
#endif

#ifndef SAFE_DELETE_ARRAY
#define SAFE_DELETE_ARRAY(p) { if (p) { delete[] (p); (p)=NULL; } }
#endif

#ifndef SAFE_RELEASE
#define SAFE_RELEASE(p) { if (p) { (p)->Release(); (p)=NULL; } }
#endif
