#define USE_SQLITE3

#include <drogon/drogon.h>
#include <filesystem>

using namespace drogon;

int main() {
	drogon::app().loadConfigFile("./config.json");

#pragma region DB Configuration

	auto dbThread = std::thread([&]()
		{
			while (!app().isRunning());

			auto db = app().getDbClient();

			db->execSqlSync(
				"create table if not exists users ("
					"id text,"
					"username text,"
					"password varchar(256),"
					"avataruri text,"
					"backgrounduri text,"
					"country text,"
					"city text,"
					"signature text,"
					"token text,"
					"primary key (id)"
				");"
			);
		});

	#pragma endregion

	app().run();

	if (dbThread.joinable())
		dbThread.join();
}
