#include <sqlite.hpp>


SQLiteDatabase::SQLiteDatabase(const char * location){
    int rc = sqlite3_open(location, &db_pointer);
    if (rc != SQLITE_OK) {
        close();
    }
}

SQLiteDatabase::SQLiteDatabase(const std::string& location) 
    : SQLiteDatabase(location.c_str()){}

SQLiteDatabase::~SQLiteDatabase(){
    close();
}


void SQLiteDatabase::close()
{
    if (db_pointer != nullptr)
    {
        sqlite3_close(db_pointer);
        db_pointer = nullptr;
    }
}



SQLiteStatement::SQLiteStatement(SQLiteDatabase &db, const char *zSql, int nByte, const char **pzTail)
    : db(db)
{
    if(sqlite3_prepare_v2(db.db_pointer, zSql, nByte, &stmt_pointer, pzTail) != SQLITE_OK){
        throw __PRETTY_FUNCTION__;
    }
}

SQLiteStatement::SQLiteStatement(SQLiteDatabase &db, const char * zSql)
    : SQLiteStatement(db, zSql, std::char_traits<char>::length(zSql))
{
    
}

SQLiteStatement::SQLiteStatement(SQLiteDatabase &db, const std::string_view zSql)
    : SQLiteStatement(db, zSql.data(), zSql.size())
{
    
}

SQLiteStatement::~SQLiteStatement(){
    finalize();
}

// SQLiteStatement& SQLiteStatement::operator=(const SQLiteStatement& other){
//     this->finalize();
//     this->db = other.db;
//     this->stmt_pointer = other.stmt_pointer;
//     return *this;
// }
