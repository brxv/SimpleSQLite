#ifndef SQLITE3_HPP
#define SQLITE3_HPP

#include <sqlite3.h>

#include <span>
#include <tuple>
#include <string_view>
#include <sstream> // std::iterator
#include <cstdint> // uint8_t, int64_t


class SQLiteDatabase;

class SQLiteStatement{

    SQLiteDatabase& db;
    sqlite3_stmt *stmt_pointer=nullptr;

    friend class SQLiteDatabase;

public:
    SQLiteStatement(SQLiteDatabase &db, const char *zSql, int nByte, const char **pzTail=NULL);
    SQLiteStatement(SQLiteDatabase &db, const char * zSql);
    SQLiteStatement(SQLiteDatabase &db, const std::string_view zSql);
    ~SQLiteStatement();

    SQLiteStatement& operator=(const SQLiteStatement& other);

    int finalize();

    int step();
    int reset();
    int clear_bindings();

    inline int bind(size_t index, const int value);
    inline int bind(size_t index, const int64_t value);
    inline int bind(size_t index, const double value);
    inline int bind(size_t index, const char *value);
    inline int bind(size_t index, const std::string_view value);
    inline int bind(size_t index, const std::span<uint8_t> value);
    inline int bind(size_t index, const std::u16string_view value);
    inline int bind(size_t index, const sqlite3_value *value);
    
    constexpr void bind(size_t index) {}
    
    template <typename A, typename... As>
    constexpr void bind(size_t index, const A& value, const As&... args)
    {
        bind(index, value);
        bind(index + 1, args...);
    }
    
    /*
    
    */
    inline void column(size_t iCol, int &value);
    inline void column(size_t iCol, int64_t &value);
    inline void column(size_t iCol, double &value);
    inline void column(size_t iCol, char *&value); // must be a null terminated string 
    inline void column(size_t iCol, std::string_view &value);
    inline void column(size_t iCol, std::span<uint8_t> &value);
    inline void column(size_t iCol, sqlite3_value *&value);
    
    constexpr void column(size_t iCol) {}
    
    template<typename B, typename... Bs>
    constexpr void column(size_t iCol, B &value, Bs &...args)
    {
        column(iCol, value);
        column(iCol + 1, args...);
    }
    
    template<typename B, typename... Bs>
    constexpr std::tuple<B, Bs...> column()
    {
        std::tuple<B, Bs...> values;
        std::apply(
            [this](auto&... tuple_values) {
                column(0, tuple_values...);
            },
            values);
        return values;
    }


    /*
    TODO:
        see example std::filesystem::directory_iterator() on how to use input_iterator 

        implement a input_iterator for SQLiteStatement
        
        https://www.fluentcpp.com/2018/05/08/std-iterator-deprecated/
        https://www.fluentcpp.com/2021/09/02/how-to-make-your-classes-compatible-with-range-for-loop/
    */
};



class SQLiteDatabase{
    sqlite3 *db_pointer;
    char *err_msg = 0;


    friend class SQLiteStatement;

public:

    SQLiteDatabase(const char * location);

    SQLiteDatabase(const std::string& location);

    ~SQLiteDatabase();

    void close();


    SQLiteStatement execute(const auto& zSql, auto&... args){
        auto stmt = SQLiteStatement(*this, zSql);
        if constexpr(sizeof...(args)>0){
            stmt.bind(1, args...);
        }
        return stmt;
    }


    template<typename B, typename... Bs>
    constexpr void executemany(const auto& zSql, const std::initializer_list<std::tuple<B,Bs...>>& values_list){
        auto stmt = SQLiteStatement(*this, zSql);
        for(const std::tuple<B,Bs...>& values: values_list)
        {
            std::apply(
                [&](const auto&... tuple_values) {
                    stmt.bind(1, tuple_values...);
                }, 
                values
            );

            stmt.step();
            stmt.clear_bindings();
            stmt.reset();
        }
    }
};


inline int SQLiteStatement::finalize(){
    return sqlite3_finalize(stmt_pointer);
}

inline int SQLiteStatement::step(){
    return sqlite3_step(stmt_pointer);
}

inline int SQLiteStatement::reset(){
    return sqlite3_reset(stmt_pointer);
}

inline int SQLiteStatement::clear_bindings(){
    return sqlite3_clear_bindings(stmt_pointer);
}


inline int SQLiteStatement::bind(size_t index, const int value)
{
    return sqlite3_bind_int(stmt_pointer, index, value);
}
inline int SQLiteStatement::bind(size_t index, const int64_t value)
{
    return sqlite3_bind_int64(stmt_pointer, index, value);
}
inline int SQLiteStatement::bind(size_t index, const double value)
{
    return sqlite3_bind_double(stmt_pointer, index, value);
}
inline int SQLiteStatement::bind(size_t index, const char *value)
{
    return sqlite3_bind_text(stmt_pointer, index, value, std::char_traits<char>::length(value), SQLITE_STATIC);
}
inline int SQLiteStatement::bind(size_t index, const std::string_view value)
{
    return sqlite3_bind_text(stmt_pointer, index, value.data(), value.size(), SQLITE_STATIC);
}
inline int SQLiteStatement::bind(size_t index, const std::span<uint8_t> value)
{
    return sqlite3_bind_blob(stmt_pointer, index, value.data(), value.size(), SQLITE_STATIC);
}
inline int SQLiteStatement::bind(size_t index, const std::u16string_view value)
{
    return sqlite3_bind_text16(stmt_pointer, index, value.data(), value.size(), SQLITE_STATIC);
}
inline int SQLiteStatement::bind(size_t index, const sqlite3_value *value)
{
    return sqlite3_bind_value(stmt_pointer, index, value);
}


inline void SQLiteStatement::column(size_t iCol, int &value)
{
    value = sqlite3_column_int(stmt_pointer, iCol);
}
inline void SQLiteStatement::column(size_t iCol, int64_t &value)
{
    value = sqlite3_column_int64(stmt_pointer, iCol);
}
inline void SQLiteStatement::column(size_t iCol, double &value)
{
    value = sqlite3_column_double(stmt_pointer, iCol);
}
inline void SQLiteStatement::column(size_t iCol, char *&value) // must be a null terminated string 
{
    value = (char *)sqlite3_column_text(stmt_pointer, iCol);
}
inline void SQLiteStatement::column(size_t iCol, std::string_view &value)
{
    value = {(char *)sqlite3_column_text(stmt_pointer, iCol), (size_t)sqlite3_column_bytes(stmt_pointer, iCol)};
}
inline void SQLiteStatement::column(size_t iCol, std::span<uint8_t> &value)
{
    value = {(uint8_t *)sqlite3_column_blob(stmt_pointer, iCol), (size_t)sqlite3_column_bytes(stmt_pointer, iCol)};
}
inline void SQLiteStatement::column(size_t iCol, sqlite3_value *&value)
{
    value = sqlite3_column_value(stmt_pointer, iCol);
}

#endif /* SQLITE3_HPP */
