/****************************************************************************
  FileName     [ dbTable.cpp ]
  PackageName  [ db ]
  Synopsis     [ Define database Table member functions ]
  Author       [ Chung-Yang (Ric) Huang ]
  Copyright    [ Copyleft(c) 2015-present LaDs(III), GIEE, NTU, Taiwan ]
****************************************************************************/

#include <iomanip>
#include <string>
#include <cctype>
#include <cassert>
#include <set>
#include <algorithm>
#include <sstream>
#include "dbTable.h"

using namespace std;

/*****************************************/
/*          Global Functions             */
/*****************************************/
ostream& operator << (ostream& os, const DBRow& r)
{
   // TODO: to print out a row.
   // - Data are seperated by a space. No trailing space at the end.
   // - Null cells are printed as '.'
  for(size_t i = 0; i < r.size(); ++i){
    if(r[i] != NONE)  os << r[i];
    else  os << ".";
    if(i != r.size()-1)  cout << " ";
  } 
  os << "\n";
   return os;
}

ostream& operator << (ostream& os, const DBTable& t)
{
   // TODO: to print out a table
   // - Data are seperated by setw(6) and aligned right.
   // - Null cells are printed as '.'
  for(size_t i = 0; i < t.nRows(); ++i){
    for(size_t j = 0; j < t.nCols(); ++j){
      if(t[i][j] != NONE) os << setw(6) << right << t[i][j];
      else os << setw(6) << right << "."; 
    }
    os << "\n";
  } 
   return os;
}

ifstream& operator >> (ifstream& ifs, DBTable& t)
{
   // TODO: to read in data from csv file and store them in a table
   // - You can assume all the data of the table are in a single line. 
  vector< string> line;
  string readin;
  getline(ifs,readin);
  stringstream ss;
  ss << readin;
  
  while(ss >> readin)  line.push_back(readin);
  for(size_t i = 0; i < line.size(); ++i){
    DBRow new_row;
    string tmp_data;
    for(size_t j = 0; j < line[i].length(); ++j){
      if(line[i][j] == ','){
        new_row.addData(!tmp_data.empty() ? atoi(tmp_data.c_str()) : NONE);
        tmp_data.clear();
        if(j == line[i].length()-1) 
          new_row.addData(NONE);
      }
      else if(j == line[i].length()-1){
        tmp_data.push_back(line[i][j]);
        new_row.addData(!tmp_data.empty() ? atoi(tmp_data.c_str()) : NONE);
      }
      else tmp_data.push_back(line[i][j]);
    }
    t.addRow(new_row);
  }

  return ifs;
}

/*****************************************/
/*   Member Functions for class DBRow    */
/*****************************************/
void
DBRow::removeCell(size_t c)
{
   // TODO
  for(size_t i = c; i < size(); ++i)
    _data[i] = _data[i+1];
  _data.pop_back();
}

/*****************************************/
/*   Member Functions for struct DBSort  */
/*****************************************/
bool
DBSort::operator() (const DBRow& r1, const DBRow& r2) const
{
   // TODO: called as a functional object that compares the data in r1 and r2
   //       based on the order defined in _sortOrder
  for(size_t i = 0; i < _sortOrder.size(); ++i){
    int col_compare = _sortOrder[i];
    if(r1[col_compare] > r2[col_compare]) {return true; break;}
    else if(r1[col_compare] < r2[col_compare]) {return false; break;}
  }
  return false;
}

/*****************************************/
/*   Member Functions for class DBTable  */
/*****************************************/
void
DBTable::reset()
{
   // TODO
  _table.clear();
}

void
DBTable::addCol(const vector<int>& d)
{
   // TODO: add a column to the right of the table. Data are in 'd'.
  for(size_t i = 0; i < nRows(); ++i)
    _table[i].addData(d[i]);
}

void
DBTable::delRow(int c)
{
   // TODO: delete row #c. Note #0 is the first row.
  //size_t n = _table.size()-1;
  for(size_t i = c; i < nRows()-1; ++i)
    _table[i] = _table[i+1];
  _table.pop_back();
}

void
DBTable::delCol(int c)
{
   // delete col #c. Note #0 is the first row.
   for (size_t i = 0, n = _table.size(); i < n; ++i)
      _table[i].removeCell(c);
}

// For the following getXXX() functions...  (except for getCount())
// - Ignore null cells
// - If all the cells in column #c are null, return NAN
// - Return "float" because NAN is a float.
float
DBTable::getMax(size_t c) const
{
   // TODO: get the max data in column #c
  float Max = (float)INT_MIN;
  for(size_t i = 0; i < nRows(); ++i)
    if(_table[i][c] > Max && _table[i][c] != NONE)
      Max = _table[i][c];
  if(Max == (float)INT_MIN) Max = NAN;
  return Max;
}

float
DBTable::getMin(size_t c) const
{
   // TODO: get the min data in column #c
  float Min = (float)INT_MAX;
  for(size_t i = 0; i < nRows(); ++i)
    if(_table[i][c] < Min)
      Min = _table[i][c];
  if(Min == (float)INT_MAX) Min = NAN;
  return Min;
}

float 
DBTable::getSum(size_t c) const
{
   // TODO: compute the sum of data in column #c
  float Sum = 0.0;
  bool all_NONE = true;
  for(size_t i = 0; i < nRows(); ++i){
    	if(_table[i][c] != NONE){
        all_NONE = false;
    		Sum += _table[i][c];
      }
  }    
  if(!all_NONE) return Sum;
  else return NAN;
}

int
DBTable::getCount(size_t c) const
{
   // TODO: compute the number of distinct data in column #c
   // - Ignore null cells
  vector< int> num;
  num.resize(0);
  for(size_t i = 0; i < nRows(); ++i){
    if(num.empty()){
      if(_table[i][c] != NONE)
        num.push_back(_table[i][c]);
    } 
    else{
      bool same = true;
      for(size_t j = 0; j < num.size(); j++) 
      	if((_table[i][c] == NONE) || _table[i][c] == num[j])
      	  same = false; 
        if(same)
          num.push_back(_table[i][c]);
    }
  }
   return num.size();
}

float
DBTable::getAve(size_t c) const
{
   // TODO: compute the average of data in column #c
  if(getSum(c) != NONE) return getSum(c)/getCount(c);
  else return NAN;
}

void
DBTable::sort(const struct DBSort& s)
{
   // TODO: sort the data according to the order of columns in 's'
	for(size_t i = 0; i < nRows()-1; ++i){
		size_t tmp_min = i;
	  for(size_t j = i; j < nRows(); ++j){
	  	if(s(_table[tmp_min],_table[j]))
	  		tmp_min = j;
	  }
	  DBRow *min_row = &_table[tmp_min];
	  DBRow *chang_row = &_table[i];
	  DBRow tmp_row(_table[i]);
	  *chang_row = _table[tmp_min];
	  *min_row = tmp_row;
	}
}

void
DBTable::printCol(size_t c) const
{
   // TODO: to print out a column.
   // - Data are seperated by a space. No trailing space at the end.
   // - Null cells are printed as '.'
  for(size_t i = 0; i < nRows(); ++i){
    if(_table[i][c] != NONE)  cout << _table[i][c];
    else  cout << ".";
    if(i != nRows()-1)  cout << " ";
  }
}

void
DBTable::printSummary() const
{
   size_t nr = nRows(), nc = nCols(), nv = 0;
   for (size_t i = 0; i < nr; ++i)
      for (size_t j = 0; j < nc; ++j)
         if (_table[i][j] != INT_MAX) ++nv;
   cout << "(#rows, #cols, #data) = (" << nr << ", " << nc << ", "
        << nv << ")" << endl;
}

