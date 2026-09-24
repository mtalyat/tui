#ifndef TuiFunction_h
#define TuiFunction_h

#include <stdio.h>
#include <string>
#include <set>
#include <functional>
#include <map>
#include <vector>
#include "glm.hpp"
#include "TuiLog.h"

#include "TuiRef.h"
#include "TuiStatement.h"

class TuiTable;
class TuiString;
class TuiFunction;

struct TuiFunctionCallData {
    TuiFunctionCallData* parentCallData = nullptr;
    TuiPointer<TuiTable> parentTable = nullptr;
    TuiPointer<TuiTable> thisTable = nullptr;
    std::map<std::string, uint32_t> localTokensByStringKey;
    std::map<uint32_t, TuiPointer<TuiRef>> locals; //need to release
    std::vector<TuiPointer<TuiTable>> transientLoopTables;
    std::vector<TuiPointer<TuiFunction>> capturedFunctions;
};


class TuiFunction : public TuiRef {
    
public: //static functions
    static TuiPointer<TuiFunction> initWithHumanReadableString(const char* str,
                                                    char** endptr,
                                                    TuiPointer<TuiTable> parent,
                                                    TuiDebugInfo* debugInfo);
    
    static bool recursivelySerializeExpression(const char* str,
                                               char** endptr,
                                               TuiExpression* expression,
                                               TuiPointer<TuiTable> parent,
                                               TuiTokenMap* tokenMap,
                                               TuiDebugInfo* debugInfo,
                                               int operatorLevel,
                                               std::string* setKey = nullptr,
                                               int* setIndex = nullptr,
                                               uint32_t subExpressionTokenStartPos = UINT32_MAX);
    
    static bool serializeFunctionBody(const char* str,
                                      char** endptr,
                                      TuiPointer<TuiTable> parent,
                                      TuiTokenMap* tokenMap,
                                      TuiDebugInfo* debugInfo,
                                      bool sharesParentScope,
                                      std::vector<TuiStatement*>* statements);
    
    static TuiStatement* serializeForStatement(const char* str,
                                                  char** endptr,
                                                  TuiPointer<TuiTable> parent,
                                                  TuiDebugInfo* debugInfo,
                                               bool sharesParentScope,
                                               bool isWhileLoop);
    
    
    
    static TuiPointer<TuiRef> runExpression(TuiExpression* expression,
                                 uint32_t* tokenPos,
                                 TuiPointer<TuiRef> result,
                                 TuiPointer<TuiTable> parent,
                                 TuiTokenMap* tokenMap,
                                 TuiFunctionCallData* callData,
                                 TuiDebugInfo* debugInfo,
                                 std::string* setKey = nullptr,
                                 int* setIndex = nullptr,
                                 TuiPointer<TuiRef>* enclosingSetRef = nullptr,
                                 std::string* subTypeAccessKey = nullptr,
                                 TuiPointer<TuiRef>* subTypeRef = nullptr);
    
    static TuiPointer<TuiRef> runStatement(TuiStatement* statement,
                                TuiPointer<TuiRef> result,
                                TuiPointer<TuiTable> parent,
                                TuiTokenMap* tokenMap,
                                TuiFunctionCallData* callData,
                                TuiDebugInfo* debugInfo,
                                bool* breakFound = nullptr);
    
    static TuiPointer<TuiRef> runStatementArray(std::vector<TuiStatement*>& statements,
                                     TuiPointer<TuiRef> result,
                                     TuiPointer<TuiTable> parent,
                                     TuiTokenMap* tokenMap,
                                     TuiFunctionCallData* callData,
                                     TuiDebugInfo* debugInfo,
                                     bool* breakFound = nullptr);

public: //class members
    TuiPointer<TuiTable> parentTable = nullptr;
    std::vector<std::string> argNames;
    std::vector<TuiStatement*> statements;
    std::function<TuiPointer<TuiRef>(TuiPointer<TuiTable> args, TuiPointer<TuiRef> existingResult, TuiFunctionCallData* incomingCallData, TuiDebugInfo* callingDebugInfo)> func;
    
    TuiTokenMap tokenMap;
    std::vector<TuiPointer<TuiTable>> retainedTransientLoopTables;
    
    TuiDebugInfoLine debugInfoLine;
    
public: //class functions
    TuiFunction(TuiPointer<TuiTable> parentTable_);
    TuiFunction(std::function<TuiPointer<TuiRef>(TuiPointer<TuiTable> args, TuiPointer<TuiRef> existingResult, TuiFunctionCallData* incomingCallData, TuiDebugInfo* callingDebugInfo)> func_);
    virtual ~TuiFunction() {};
    
    virtual TuiPointer<TuiRef> copy() //NOTE! This is not a true copy, copy is called internally when assigning vars, but tables, function, and userdata are treated like pointers
    {
        return createPointerFromThis();
    }
    
    void releaseAndRemoveTransientLoopTables();
    
    TuiPointer<TuiFunction> trueCopy() //assumed to only be used for function construction from a pre-serialized prototype
    {
        TuiPointer<TuiFunction> copied = Tui::createPointer<TuiFunction>(parentTable);
        copied->argNames = argNames;
        copied->statements = statements;
        for(TuiStatement* statement : copied->statements)
        {
            statement->refCount++;
        }
        copied->func = func;
        copied->tokenMap = tokenMap;
        copied->debugInfoLine = debugInfoLine;
        return copied;
    }
    
    virtual uint8_t type() { return Tui_ref_type_FUNCTION; }
    virtual std::string getTypeName() {return "function";}
    virtual std::string getStringValue() {return "function";}
    virtual bool isEqual(TuiPointer<TuiRef> other) {return other.get() == this;}
    
    virtual bool boolValue() {return true;}
    
    TuiPointer<TuiRef> call(TuiPointer<TuiTable> args,
                 TuiPointer<TuiRef> existingResult,
                 TuiFunctionCallData* incomingCallData,
                 TuiDebugInfo* callingDebugInfo);
    
    TuiPointer<TuiRef> runTableConstruct(TuiPointer<TuiTable> state,
                 TuiPointer<TuiRef> existingResult,
                 TuiDebugInfo* callingDebugInfo);
    
    TuiPointer<TuiRef> call(const std::string& debugName,
                 TuiPointer<TuiRef> arg1 = nullptr,
                 TuiPointer<TuiRef> arg2 = nullptr,
                 TuiPointer<TuiRef> arg3 = nullptr,
                 TuiPointer<TuiRef> arg4 = nullptr,
                 TuiPointer<TuiRef> arg5 = nullptr,
                 TuiPointer<TuiRef> arg6 = nullptr,
                 TuiPointer<TuiRef> arg7 = nullptr,
                 TuiPointer<TuiRef> arg8 = nullptr);
    
    TuiPointer<TuiRef> call(TuiFunctionCallData* incomingCallData,
                              TuiDebugInfo* callingDebugInfo,
                              TuiPointer<TuiRef> arg1 = nullptr,
                              TuiPointer<TuiRef> arg2 = nullptr,
                              TuiPointer<TuiRef> arg3 = nullptr,
                              TuiPointer<TuiRef> arg4 = nullptr,
                              TuiPointer<TuiRef> arg5 = nullptr,
                              TuiPointer<TuiRef> arg6 = nullptr,
                              TuiPointer<TuiRef> arg7 = nullptr,
                              TuiPointer<TuiRef> arg8 = nullptr);
    
    //void call(TuiPointer<TuiTable> args, std::function<void(TuiPointer<TuiRef>)> callback); //todo async
    
    virtual void serializeBinaryToBuffer(std::string& buffer, int* currentOffset)
    {
        TuiError("TuiFunction does not support binary serialization");
    }
    
private:
};

#endif
