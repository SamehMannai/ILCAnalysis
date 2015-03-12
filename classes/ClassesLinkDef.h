
/** ClassesLinkDef
 *
 *  Lists classes to be included in cint dicitonary
 *
 *  $Date: 2008-07-08 12:01:52 $
 *  $Revision: 1.2 $
 *
 *
 *  \author P. Demin - UCL, Louvain-la-Neuve
 *
 */

#include "classes/SortableObject.h"
#include "classes/ILCClasses.h"

#ifdef __CINT__

#pragma link off all globals;
#pragma link off all classes;
#pragma link off all functions;

#pragma link C++ class SortableObject+;

#pragma link C++ class Event+;
#pragma link C++ class CaloHit+;
#pragma link C++ class MyResult+;

#endif

