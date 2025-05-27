#include <iostream>
#include <sqlite.hpp>

int main()
{

    SQLiteDatabase db{"test.db"};

    db.execute("CREATE TABLE IF NOT EXISTS test_table (id INTEGER PRIMARY KEY, value TEXT)").step();
    
    db.execute("INSERT INTO test_table (value) VALUES(?)", "Hello Word!").step();

    // Don't need to step() on executemany()
    db.executemany<const char*>("INSERT INTO test_table (value) VALUES(?)", 
        {
            {"Hello Word!"}, 
            {"Hello Word!"}
        }
    );

    auto stmt = db.execute("SELECT * FROM test_table");

    // Don't need to step() on fetchall()
    for( auto [id, value]: stmt.fetchall<int, std::string_view>() ){
        std::cout << id << ' ' << value << "\n";
    }

    for( auto [id, value]: stmt.fetchall<int, std::string_view>() ){
        std::cout << id << ' ' << value << "\n";
    }

    return 0;

    // int rc;
    // while ((rc = stmt.step()) == SQLITE_ROW)
    // {
    //     auto [id, value] = stmt.column<int, std::string_view>();
    //     std::cout << id << ' ' << value << "\n";
    // }
    // return 0;
}
