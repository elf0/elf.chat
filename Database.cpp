//License: Public Domain
//Author: elf
//EMail: elf198012@gmail.com

#include "Database.h"

Database::Database(){
}

Database::~Database(){
    if(IsOpen())
        Close();
}

Bool Database::Open(const std::string &strFilePathName){
    int rc = sqlite3_open(strFilePathName.c_str(), &_pDb);
    if(rc != SQLITE_OK)
        return false;

    if(!PrepareTables()){
        Close();
        return false;
    }

    if(!PrepareStatements()){
        Close();
        return false;
    }

    return true;
}

void Database::Close(){
    sqlite3_finalize(_pstEnglish);
    _pstEnglish = nullptr;

    sqlite3_finalize(_pstEnglishId);
    _pstEnglishLike = nullptr;

    sqlite3_finalize(_pstEnglishLike);
    _pstEnglishLike = nullptr;

    sqlite3_close(_pDb);
    _pDb = nullptr;
}

U64 Database::Find(const std::string &strText){
    U64 nId = 0;
    sqlite3_bind_text(_pstEnglish, 1, strText.c_str(), strText.size(), SQLITE_TRANSIENT);
    while(sqlite3_step(_pstEnglish) == SQLITE_ROW){
        nId = sqlite3_column_int64(_pstEnglish, 0);
        break;
    }
    sqlite3_reset(_pstEnglish);
    return nId;
}

Bool Database::FindPrefix(const std::string &strPrefix, std::vector<std::string> &vTexts){
    std::string str = strPrefix + '%';
    int i = 0;
    sqlite3_bind_text(_pstEnglishLike, 1, str.c_str(), str.size(), SQLITE_TRANSIENT);
    while(sqlite3_step(_pstEnglishLike) == SQLITE_ROW){
        const char *pText = (const char *)sqlite3_column_text(_pstEnglishLike, 1);
        int nBytes = sqlite3_column_bytes(_pstEnglishLike, 1);
        vTexts[i++] = std::string(pText, nBytes);
    }
    sqlite3_reset(_pstEnglishLike);
    return false;
}

Bool Database::FindId(U64 nId, std::string &strText){
    sqlite3_bind_int64(_pstEnglishId, 1, nId);
    while(sqlite3_step(_pstEnglishId) == SQLITE_ROW){
        const char *pText = (const char *)sqlite3_column_text(_pstEnglishId, 0);
        int nBytes = sqlite3_column_bytes(_pstEnglishId, 0);
        strText.assign(pText, nBytes);
        sqlite3_reset(_pstEnglishId);
        return true;
    }
    sqlite3_reset(_pstEnglishId);
    return false;
}

Bool Database::PrepareTables(){
    if(!Begin())
        return false;

    int rc = sqlite3_exec(_pDb, "Create table if not exists t_english(f_id integer primary key, f_text text not null unique)", 0, 0, 0);
    if(rc != SQLITE_OK){
        Rollback();
        return false;
    }

    return Commit();
}

Bool Database::PrepareStatements(){
    int rc = sqlite3_prepare_v2(_pDb, "Select f_id from t_english where f_text = ?", -1, &_pstEnglish, 0);
    if(rc != SQLITE_OK)
        return false;

    rc = sqlite3_prepare_v2(_pDb, "Select f_text from t_english where f_id = ?", -1, &_pstEnglishId, 0);
    if(rc != SQLITE_OK)
        return false;

    rc = sqlite3_prepare_v2(_pDb, "Select * from t_english where f_text like ? limit 10", -1, &_pstEnglishLike, 0);
    if(rc != SQLITE_OK)
        return false;
    return true;
}

Bool Database::Begin(){
    return sqlite3_exec(_pDb, "Begin", 0, 0, 0) == SQLITE_OK;
}

Bool Database::Commit(){
    return sqlite3_exec(_pDb, "Commit", 0, 0, 0) == SQLITE_OK;
}

Bool Database::Rollback(){
    return sqlite3_exec(_pDb, "Rollback", 0, 0, 0) == SQLITE_OK;
}
