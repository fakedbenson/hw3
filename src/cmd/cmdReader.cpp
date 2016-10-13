/****************************************************************************
  FileName     [ cmdReader.cpp ]
  PackageName  [ cmd ]
  Synopsis     [ Read the command from the standard input or dofile ]
  Author       [ Chung-Yang (Ric) Huang ]
  Copyright    [ Copyleft(c) 2007-present LaDs(III), GIEE, NTU, Taiwan ]
****************************************************************************/
#include <iostream>
#include <cassert>
#include <cstring>
#include "util.h"
#include "cmdParser.h"

using namespace std;

//----------------------------------------------------------------------
//    Extrenal funcitons
//----------------------------------------------------------------------
void mybeep();


//----------------------------------------------------------------------
//    Member Function for class CmdParser
//----------------------------------------------------------------------
bool
CmdParser::readCmd(istream& istr)
{
   resetBufAndPrintPrompt();

   // THIS IS EQUIVALENT TO "readCmdInt()" in HW#2
   bool newCmd = false;
   while (!newCmd) {
      ParseChar pch = getChar(istr);
      if (pch == INPUT_END_KEY) {
         if (_dofile != 0)
            closeDofile();
         break;
      }
      switch (pch) {
         case LINE_BEGIN_KEY :
         case HOME_KEY       : moveBufPtr(_readBuf); break;
         case LINE_END_KEY   :
         case END_KEY        : moveBufPtr(_readBufEnd); break;
         case BACK_SPACE_KEY : // YOUR HW#2 TODO
                                if(_readBufPtr > _readBuf)
                                {cout << "\b"; _readBufPtr --; deleteChar();}
                              else mybeep(); break;
         case DELETE_KEY     : deleteChar(); break;
         case NEWLINE_KEY    : newCmd = addHistory();
                               cout << char(NEWLINE_KEY);
                               if (!newCmd) resetBufAndPrintPrompt();
                               break;
         case ARROW_UP_KEY   : moveToHistory(_historyIdx - 1); break;
         case ARROW_DOWN_KEY : moveToHistory(_historyIdx + 1); break;
         case ARROW_RIGHT_KEY: // YOUR HW#2 TODO
                                if(_readBufEnd > _readBufPtr) moveBufPtr(_readBufPtr + 1);
                              else mybeep(); break;
         case ARROW_LEFT_KEY : // YOUR HW#2 TODO
                                if(_readBufPtr > _readBuf) moveBufPtr(_readBufPtr - 1);
                              else mybeep(); break;
         case PG_UP_KEY      : moveToHistory(_historyIdx - PG_OFFSET); break;
         case PG_DOWN_KEY    : moveToHistory(_historyIdx + PG_OFFSET); break;
         case TAB_KEY        : // THIS IS DIFFERENT FROM HW#2
                               { char tmp = *_readBufPtr; *_readBufPtr = 0;
                               string str = _readBuf; *_readBufPtr = tmp;
                               listCmd(str);
                               break; }
                              //insertChar(' ', TAB_POSITION - (_readBufPtr - _readBuf ) % TAB_POSITION); break;
         case INSERT_KEY     : // not yet supported; fall through to UNDEFINE
         case UNDEFINED_KEY:   mybeep(); break;
         default:  // printable character
            insertChar(char(pch)); break;
      }
      #ifdef TA_KB_SETTING
      taTestOnly();
      #endif
   }
   return newCmd;
}


// This function moves _readBufPtr to the "ptr" pointer
// It is used by left/right arrowkeys, home/end, etc.
//
// Suggested steps:
// 1. Make sure ptr is within [_readBuf, _readBufEnd].
//    If not, make a beep sound and return false. (DON'T MOVE)
// 2. Move the cursor to the left or right, depending on ptr
// 3. Update _readBufPtr accordingly. The content of the _readBuf[] will
//    not be changed
//
// [Note] This function can also be called by other member functions below
//        to move the _readBufPtr to proper position.
bool
CmdParser::moveBufPtr(char* const ptr)
{
   // THIS IS YOUR HW#2 TODO
  if(_readBufPtr > ptr){
    while(_readBufPtr > ptr){
      cout << "\b";
      _readBufPtr --;
    }
  }
  else if(ptr > _readBufPtr){
    while(ptr > _readBufPtr){
      cout << *_readBufPtr; 
       _readBufPtr ++;
    }
  }
  return true;
}


// [Notes]
// 1. Delete the char at _readBufPtr
// 2. mybeep() and return false if at _readBufEnd
// 3. Move the remaining string left for one character
// 4. The cursor should stay at the same position
// 5. Remember to update _readBufEnd accordingly.
// 6. Don't leave the tailing character.
// 7. Call "moveBufPtr(...)" if needed.
//
// For example,
//
// cmd> This is the command
//              ^                (^ is the cursor position)
//
// After calling deleteChar()---
//
// cmd> This is he command
//              ^
//
bool
CmdParser::deleteChar()
{
   // THIS IS YOUR HW#2 TODO
  if(_readBufPtr < _readBufEnd){
    char *_tmpPtr = _readBufPtr;
    _readBufPtr ++;
    moveBufPtr(_readBufEnd);
    cout << " ";
    _readBufPtr = _tmpPtr;
    while(_readBufPtr < _readBufEnd){
      *_readBufPtr = (*(_readBufPtr+1));
      _readBufPtr ++;
    }
    moveBufPtr(_tmpPtr);
    _readBufEnd --;
  }
    return true;
}

// 1. Insert character 'ch' for "repeat" times at _readBufPtr
// 2. Move the remaining string right for "repeat" characters
// 3. The cursor should move right for "repeats" positions afterwards
// 4. Default value for "repeat" is 1. You should assert that (repeat >= 1).
//
// For example,
//
// cmd> This is the command
//              ^                (^ is the cursor position)
//
// After calling insertChar('k', 3) ---
//
// cmd> This is kkkthe command
//                 ^
//
void
CmdParser::insertChar(char ch, int repeat)
{
   // THIS IS YOUR HW#2 TODO
  assert(repeat >= 1);
  char _tmpCh = ch;
  for(int i = 0; i < repeat; i++){  
    char *_tmpPtr = _readBufPtr;
    while(_readBufEnd > _readBufPtr){
        char _tmp = *_readBufPtr;
        *_readBufPtr = ch;
        _readBufPtr ++;
        cout << ch;
        ch = _tmp;
    }
    *_readBufPtr = ch;
    cout << ch;
    _readBufPtr ++;
    _tmpPtr ++;
    moveBufPtr(_tmpPtr);
    _readBufEnd ++;
    ch = _tmpCh;
  }   
}

// 1. Delete the line that is currently shown on the screen
// 2. Reset _readBufPtr and _readBufEnd to _readBuf
// 3. Make sure *_readBufEnd = 0
//
// For example,
//
// cmd> This is the command
//              ^                (^ is the cursor position)
//
// After calling deleteLine() ---
//
// cmd>
//      ^
//
void
CmdParser::deleteLine()
{
   // THIS IS YOUR HW#2 TODO
  moveBufPtr(_readBufEnd);
  while(_readBufEnd != _readBuf){
    *_readBufEnd = 0;
    _readBufEnd --;
    cout << "\b \b";
  }
  *_readBuf = 0;
  cout << " \b";
  _readBufPtr = _readBufEnd = _readBuf;   
}


// Reprint the current command to a newline
// cursor should be restored to the original location
void
CmdParser::reprintCmd()
{
   // THIS IS NEW IN HW#3
   cout << endl;
   char *tmp = _readBufPtr;
   _readBufPtr = _readBufEnd;
   printPrompt(); cout << _readBuf;
   moveBufPtr(tmp);
}


// This functions moves _historyIdx to index and display _history[index]
// on the screen.
//
// Need to consider:
// If moving up... (i.e. index < _historyIdx)
// 1. If already at top (i.e. _historyIdx == 0), beep and do nothing.
// 2. If at bottom, temporarily record _readBuf to history.
//    (Do not remove spaces, and set _tempCmdStored to "true")
// 3. If index < 0, let index = 0.
//
// If moving down... (i.e. index > _historyIdx)
// 1. If already at bottom, beep and do nothing
// 2. If index >= _history.size(), let index = _history.size() - 1.
//
// Assign _historyIdx to index at the end.
//
// [Note] index should not = _historyIdx
//
void
CmdParser::moveToHistory(int index)
{
   // THIS IS YOUR HW#2 TODO
  int size = _history.size();
  string tmp_history;
  bool up, down;
  if(index < _historyIdx) {up = true; down = false;}
  else if(index > _historyIdx) {down = true; up = false;}
  else up = down = false;

  if(index < 0) index = 0;
  if(index >= size) index = size-1;

  if(up){
    if(_historyIdx == 0)  mybeep();
    else{
      if(_historyIdx == size){
          tmp_history.assign(_readBuf);
          _history.push_back(tmp_history);
          _tempCmdStored = true;
      }
      _historyIdx = index;
      retrieveHistory();
    }
  }
  else if(down){
    if(_historyIdx == size)  mybeep();
    else{
      _historyIdx = index;
      retrieveHistory();
      if(_historyIdx == size-1 && _tempCmdStored) 
        {_history.pop_back(); _tempCmdStored = false;}
    }
  }
}


// This function adds the string in _readBuf to the _history.
// The size of _history may or may not change. Depending on whether 
// there is a temp history string.
//
// 1. Remove ' ' at the beginning and end of _readBuf
// 2. If not a null string, add string to _history.
//    Be sure you are adding to the right entry of _history.
// 3. If it is a null string, don't add anything to _history.
// 4. Make sure to clean up "temp recorded string" (added earlier by up/pgUp,
//    and reset _tempCmdStored to false
// 5. Reset _historyIdx to _history.size() // for future insertion
// 6. Reset _readBufPtr and _readBufEnd to _readBuf
// 7. Make sure *_readBufEnd = 0 ==> _readBuf becomes null string
//
bool
CmdParser::addHistory()
{
   // THIS IS SIMILAR TO addHistory in YOUR HW#2 TODO
   // HOWEVER, return true if a new command is entered
   // return false otherwise
     string history_str;
  
  int leading = 0;
  int ending = (_readBufEnd - _readBuf)-1;
  while(_readBuf[leading] == ' ') leading ++;
  while(_readBuf[ending] == ' ')  ending --;
  for(int i = leading; i <= ending; i++)
    history_str.push_back(_readBuf[i]);

  if(_tempCmdStored) _history.pop_back();
  if(history_str.length() != 0) _history.push_back(history_str);

  while(_readBufEnd != _readBuf){
    *_readBufEnd = 0;
    _readBufEnd --;
  }
  *_readBuf = 0;
  _readBufPtr = _readBuf;

  _tempCmdStored = false;
  _historyIdx = _history.size();

   bool newCmd = false;
   if(history_str.length() != 0) newCmd = true;
   return newCmd;
}


// 1. Replace current line with _history[_historyIdx] on the screen
// 2. Set _readBufPtr and _readBufEnd to end of line
//
// [Note] Do not change _history.size().
//
void
CmdParser::retrieveHistory()
{
   deleteLine();
   strcpy(_readBuf, _history[_historyIdx].c_str());
   cout << _readBuf;
   _readBufPtr = _readBufEnd = _readBuf + _history[_historyIdx].size();
}
