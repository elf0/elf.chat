#ifndef DATABASE_H
#define DATABASE_H

//License: Public Domain
//Author: elf
//EMail: elf198012@gmail.com

#include <vector>
#include <string>
#include "Type.h"
#include "sqlite3.h"

class Database
{
public:
    Database();
    ~Database();
    Bool Open(const std::string &strFilePathName);
    void Close();
    Bool IsOpen(){
        return _pDb != nullptr;
    }

    U64 Find(const std::string &strText);
    Bool FindPrefix(const std::string &strPrefix, std::vector<std::string> &vTexts);
    Bool FindId(U64 nId, std::string &strText);
private:
    inline Bool PrepareTables();
    inline Bool PrepareStatements();
    inline Bool Begin();
    inline Bool Commit();
    inline Bool Rollback();
    sqlite3 *_pDb = nullptr;
    sqlite3_stmt *_pstEnglish = nullptr;
    sqlite3_stmt *_pstEnglishId = nullptr;
    sqlite3_stmt *_pstEnglishLike = nullptr;
};

#endif // DATABASE_H
