
#ifndef __TuiBuiltInFunctions__
#define __TuiBuiltInFunctions__

#include <string>
#include <chrono>

#include "TuiPointer.h"

class TuiTable;
class TuiFunction;

namespace Tui {


//TODO WARNING! This is not fully implemented, not to be trusted yet
TuiPointer<TuiTable> initSafeRootTable(TuiPointer<TuiFunction> permissionCallbackFunction = nullptr, const std::string& sandBoxDir = ""); //pass permissionCallbackFunction to selectively give permission for some sensitive functions. Pass sandbox dir to restrict all file operations to within that directory.
// eg. in tui: permissionCallbackFunction = function(functionName, args, hasPermissionCallback) {
//      if(functionName == "system")
//      {
//          hasPermissionCallback(false)
//      }
//      else
//      {
//          hasPermissionCallback(true)
//      }
//  }

TuiPointer<TuiTable> initRootTable();

inline double nowTime()
{
    auto nowTime = std::chrono::system_clock::now();
    return std::chrono::duration_cast<std::chrono::microseconds>(nowTime.time_since_epoch()).count() / 1000000.0;
}


static inline const TuiPointer<TuiTable>& getRootTable()
{
    thread_local TuiPointer<TuiTable> rootTable = Tui::initRootTable();
    return rootTable;
}


void addBaseFunctions(const TuiPointer<TuiTable>& rootTable, TuiPointer<TuiFunction> permissionCallbackFunction = nullptr);
void addStringTable(const TuiPointer<TuiTable>& rootTable);
void addTimeTable(const TuiPointer<TuiTable>& rootTable);
void addTableTable(const TuiPointer<TuiTable>& rootTable);
void addMathTable(const TuiPointer<TuiTable>& rootTable);
void addFileTable(const TuiPointer<TuiTable>& rootTable, const std::string& sandBoxDir = "");
void addDebugTable(const TuiPointer<TuiTable>& rootTable);

}

#endif
