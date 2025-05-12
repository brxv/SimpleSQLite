#include <iostream>
#include <sqlite.hpp>

int main()
{

    SQLiteDatabase db{"test.db"};

    db.execute("CREATE TABLE IF NOT EXISTS test_table (id INTEGER PRIMARY KEY, value TEXT)").step();
    
    db.execute("INSERT INTO test_table (value) VALUES(?)", "Hello Word!").step();

    db.executemany<const char*>("INSERT INTO test_table (value) VALUES(?)", 
        {
            {"Hello Word!"}, 
            {"Hello Word!"}
        }
    );

    // Don't need stmt.step() on executemany()

    auto stmt = db.execute("SELECT * FROM test_table");

    int rc;
    while ((rc = stmt.step()) == SQLITE_ROW)
    {
        auto [id, value] = stmt.column<int, std::string_view>();
        std::cout << id << ' ' << value << "\n";
    }
    return 0;
}
